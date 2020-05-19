//
// Created by WTF on 2020/4/30.
//

#include <unistd.h>
#include <cstring>
#include <functional>
#include <cstdlib>
#include <fcntl.h>
#include <cassert>
#include "TcpConnection.h"
#include "Logging.h"
#include "Util.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses"


using std::min;
using std::max;

TcpConnection::TcpConnection(const string &peerAddr, int fd, EventLoop *loop):
    peerAddr_(peerAddr),
    channel_(fd),
    loop_(loop),
    parsingState_(PARSE_HEADLINE),
    connState_(ESTABLISHED),
    timer_(now() + KEEPALIVE)
{
    channel_.setPollInFunc(std::bind(&TcpConnection::readData, this));
    channel_.enablePollIn();
    channel_.setPollOutFunc(std::bind(&TcpConnection::writeData, this));
    loop_->addTask(std::bind(&EventLoop::establishConn, loop_, peerAddr_, TcpConnectionPtr(this)));
}

void TcpConnection::readData() {
    char buf[65537];
    strcpy(buf, inBuffer_.c_str());
    int sz = inBuffer_.size();
    int cnt = read(channel_.getFd(), buf + sz, sizeof(buf) - sz - 1);
    buf[cnt + sz] = '\0';
    LOG << peerAddr_ << " read " << cnt << " bytes";
    if(cnt < 0){
        assert(errno != EAGAIN && errno != EWOULDBLOCK);
        LOG << strerror(errno);
        connState_ = CLOSED;
        loop_->removeConn(peerAddr_);
        return;
    }
    if(cnt == 0){
        connState_ = CLOSE_WAIT;
    }
    if(connState_ == ESTABLISHED){
        parseData(buf, cnt + sz);
    }
    // peerclose
    if(!channel_.pollOutEnabled()){
        writeData();
        if(!outBuffer_.empty()){
            channel_.enablePollOut();
            loop_->updateChannel(channel_);
        }
    }
}

void TcpConnection::parseData(char *buf, int size) {
    char *pos = buf;
    char *prevPos, *tmpPos, *crlfPos;
    char *endPos = buf + size;
    while(1){
        if(parsingState_ == PARSE_HEADLINE){
            crlfPos = strstr(pos, "\r\n");
            if(!crlfPos){
                break;
            }
            request_ = std::make_shared<HttpRequest>();
            // speed up find str
            *crlfPos = '\0';
            tmpPos = nullptr;
            tmpPos = max(tmpPos, strstr(pos, "GET"));
            tmpPos = max(tmpPos, strstr(pos, "HEAD"));
            tmpPos = max(tmpPos, strstr(pos, "POST"));

            pos = tmpPos;
            prevPos = pos;
            // METHOD
            if(*pos == 'G'){
                // GET
                request_->setMethod(GET);
                pos += 3;
            }
            else if(*pos == 'H'){
                // head
                request_->setMethod(HEAD);
                pos += 4;
            }
            else if(*pos == 'P'){
                // post
                request_->setMethod(POST);
                pos += 4;
            }
            if(*pos != ' '){
                // parse error
                pos = crlfPos + 2;
                continue;
            }
            ++pos;
            // PATH
            char *versionPos = crlfPos - strlen("HTTP/1.1");
            request_->setPath(string(pos, versionPos - 1));
            // VERSION
            if(!strcmp(versionPos, "HTTP/1.0")){
                request_->setVersion(VERSION_1_0);
            }
            else if(!strcmp(versionPos, "HTTP/1.1")){
                request_->setVersion(VERSION_1_1);
            }
            else{
                pos = crlfPos + 2;
                continue;
            }
            pos = crlfPos + 2;
            parsingState_ = PARSE_HEADERS;
            request_->receiveBytes(pos - prevPos);
        }
        else if(parsingState_ == PARSE_HEADERS){
            crlfPos = strstr(pos, "\r\n");
            if(!crlfPos){
                break;
            }
            prevPos = pos;
            if(!strncmp(pos, "\r\n", 2)){
                parsingState_ = PARSE_BODY;
                pos += 2;
                request_->receiveBytes(2);
                continue;
            }
            *crlfPos = '\0';
            tmpPos = strchr(pos, ':');
            request_->addHeader(string(pos, tmpPos), string(tmpPos + 2, crlfPos));
            pos = crlfPos + 2;
            request_->receiveBytes(pos - prevPos);
        }
        else{
            assert(parsingState_ == PARSE_BODY);
            string contentLenStr = request_->getHeader("Content-Length");
            if(!contentLenStr.empty()){
                char *endptr;
                int contentLen = strtol(contentLenStr.c_str(), &endptr, 10);
                if(errno != 0 || *endptr != '\0'){
                    // parse error
                    parsingState_ = PARSE_HEADLINE;
                    pos = endptr;
                    continue;
                }
                int bodyLen = contentLen - request_->getReceivedLen();
                if(pos + bodyLen > endPos){
                    break;
                }
                // success
                request_->setBody(string(pos, pos + bodyLen));
                pos += bodyLen;
            }
            parsingState_ = PARSE_HEADLINE;
            requestList_.push_back(request_);
            request_ = nullptr;
        }
    }
    inBuffer_ = string(pos, endPos);
}

void TcpConnection::handleResponse() {
    if(requestList_.empty()){
        return;
    }
    string writeBuf;
    writeBuf.reserve(65536 + 4096);
    if(outBuffer_.size() <= 65536 / 2){
        writeBuf += outBuffer_;
        while(writeBuf.size() < 65536 && !requestList_.empty()){
            HttpRequestPtr re = requestList_.front();
            requestList_.pop_front();
            if(re->getVersion() == VERSION_1_0){
                writeBuf += "HTTP/1.0 ";
            }
            else if(re->getVersion() == VERSION_1_1){
                writeBuf += "HTTP/1.1 ";
            }
            string path = re->getPath();
            path = (path == "/") ? "index.html" : path.substr(1);
            int fd = open(path.c_str(), O_RDONLY);
            if(fd < 0){
                writeBuf += "404 Not Found\r\n";
                writeBuf += "Server: 0x34hz's WebServer\r\n";
                writeBuf += "\r\n";
                writeBuf += "<h1 align=\"center\">404 Not Found!</h1>";
            }
            else{
                writeBuf += "200 OK\r\n";
                writeBuf += "Server: 0x34hz's WebServer\r\n";
                writeBuf += "\r\n";
                char fileBuf[4097];
                while(1){
                    int cnt = read(fd, fileBuf, sizeof(fileBuf) - 1);
                    if(cnt <= 0){
                        break;
                    }
                    fileBuf[cnt] = '\0';
                    writeBuf += fileBuf;
                }
                close(fd);
            }
        }
        swap(outBuffer_, writeBuf);
    }
}

void TcpConnection::handleWrite() {
// sock.enablePollOut();
    if(connState_ == CLOSED){
        return;
    }
    if(!outBuffer_.empty()){
        int cnt = write(channel_.getFd(), outBuffer_.c_str(), outBuffer_.size());
        if(cnt < 0){
            assert(errno != EAGAIN && errno != EWOULDBLOCK);
            connState_ = CLOSED;
            // fixme: delete 'this' in object
            loop_->removeConn(peerAddr_);
            return;
        }
        assert(cnt != 0);
        outBuffer_ = outBuffer_.substr(cnt);
    }
    if(connState_ == CLOSE_WAIT && requestList_.empty()){
        loop_->addTask(std::bind(&EventLoop::removeConn, loop_, peerAddr_));
        connState_ = CLOSED;
    }
}

void TcpConnection::writeData() {
    updateTimer();
    handleResponse();
    handleWrite();
}

void TcpConnection::handleKeepAliveTimeOut(const Timer &timer){
    auto conn = timer.getConnection().lock();
    if(!conn){
        return;
    }
    assert(conn->outBuffer_.empty() && conn->requestList_.empty());
    conn->connState_ = CLOSED;
    conn->loop_->addTask(std::bind(&EventLoop::removeConn, conn->loop_, conn->peerAddr_));
    return;
}

void TcpConnection::updateTimer() {
    loop_->removeTimer(timer_);
    timer_.setTimeOut(now() + KEEPALIVE);
    loop_->addTimer(timer_);
}

void TcpConnection::handleClose() {

}

#pragma clang diagnostic pop