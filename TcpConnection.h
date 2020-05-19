//
// Created by WTF on 2020/4/30.
//

#ifndef WEBSERVER_TCPCONNECTION_H
#define WEBSERVER_TCPCONNECTION_H

#include "Channel.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include "HttpRequest.h"
#include "Timer.h"
#include <string>
#include <list>

using std::string;
using std::list;

class TcpConnection {
    enum ConnectionState{
        ESTABLISHED,
        CLOSE_WAIT,
        CLOSED,
    };
    string peerAddr_;
    Channel channel_;
    EventLoop *loop_;
    string inBuffer_;
    string outBuffer_;
    ParsingState parsingState_;
    ConnectionState connState_;
    list<HttpRequestPtr> requestList_;
    HttpRequestPtr request_;
    Timer timer_;
public:
    TcpConnection(const string &peerAddr, int fd, EventLoop *loop);
//    ~TcpConnection() { LOG << "destruct"; }
    void readData();
    void writeData();
    void parseData(char *buf, int size);
    void handleWrite();
    void handleResponse();
    void handleClose();
    string &getPeerAddr() { return peerAddr_; }
    Channel &getChannel() { return channel_; }
    Timer &getTimer() { return timer_; }
    void updateTimer();
    static void handleKeepAliveTimeOut(const Timer &timer);
};


#endif //WEBSERVER_TCPCONNECTION_H
