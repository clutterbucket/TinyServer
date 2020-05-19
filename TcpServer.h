//
// Created by WTF on 2020/4/30.
//

#ifndef WEBSERVER_TCPSERVER_H
#define WEBSERVER_TCPSERVER_H

#include "EventLoop.h"
#include <vector>

class TcpServer {
    Channel channel_;
    EventLoop *ioLoop_;
    int threadNum_;
    std::vector<EventLoop *> threadLoops_;
    int loopIndex_;
public:
    TcpServer(const char *ipStr, const char *portStr, EventLoop *loop, int threadNum = 1);
    void acceptConn();
};


#endif //WEBSERVER_TCPSERVER_H
