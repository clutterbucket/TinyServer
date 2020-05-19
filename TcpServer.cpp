//
// Created by WTF on 2020/4/30.
//

#include "TcpServer.h"
#include "TcpConnection.h"
#include "Util.h"
#include "Logging.h"
#include "Thread.h"
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

TcpServer::TcpServer(const char *ipStr, const char *portStr, EventLoop *loop, int threadNum) :
    channel_(createSocketfd()),
    ioLoop_(loop),
    threadNum_(threadNum),
    loopIndex_(0)
{
    if(threadNum_ <= 0){
        LOG_FATAL << "Invalid threadNum";
    }

    threadLoops_.push_back(ioLoop_);
    for(int i = 1; i < threadNum_; ++i){
        threadLoops_.push_back((new Thread)->getLoop());
    }

    int opt = 1;
    setsockopt(channel_.getFd(), SOL_SOCKET,SO_REUSEADDR,
            (const void *)&opt, sizeof(opt) );
    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    int conn = inet_pton(AF_INET, ipStr, &servaddr.sin_addr);
    if(conn <= 0){
        LOG_ERR;
    }
    char *end;
    long val = strtol(portStr, &end, 10);
    if(errno != 0 || val < 0 || val > 65535 || *end != '\0'){
        LOG_FATAL << "Invalid port";
    }
    servaddr.sin_port = htons((uint16_t)val);

    int fd = channel_.getFd();
    int flags = fcntl(fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
    conn = bind(fd, (SA *)&servaddr, sizeof(servaddr));
    if(conn < 0){
        LOG_ERR;
    }
    listen(fd, 100);
    channel_.setPollInFunc(std::bind(&TcpServer::acceptConn, this));
    channel_.enablePollIn();
    loop->addChannel(channel_);
}

void TcpServer::acceptConn() {
    while(1){
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);
        int connfd = accept(channel_.getFd(), (SA *) &cliaddr, &len);
        if (connfd < 0) {
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                break;
            }
            LOG_ERR;
        }
        char ipStr[16];
        uint16_t port = ntohs(cliaddr.sin_port);
        inet_ntop(AF_INET, &cliaddr.sin_addr, ipStr, sizeof(ipStr));
        string peerAddr = string(ipStr) + ':' + std::to_string(port);
        LOG << peerAddr << " connected";
        // loop will take control of the connection
        new TcpConnection(peerAddr, connfd, threadLoops_[loopIndex_]);
        loopIndex_ = (loopIndex_ == threadNum_ - 1) ? 0 : loopIndex_ + 1;
    }
}
