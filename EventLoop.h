//
// Created by WTF on 2020/4/30.
//

#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H

#include "Channel.h"
#include "TaskQueue.h"
#include "TimerQueue.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <set>

using std::string;

class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::weak_ptr<TcpConnection> wTcpConnectionPtr;

class EventLoop {
    int epfd_;
    std::unordered_map<string, TcpConnectionPtr> connections_;
    TaskQueue taskQueue_;
    TimerQueue timerQueue_;
    bool timeOut_;
    bool taskCome_;
public:
    int getSize() { return connections_.size(); }
    EventLoop();
    void addChannel(const Channel &channel);
    void removeChannel(const Channel &channel);
    void updateChannel(const Channel &channel);

    void establishConn(const string &peerAddr, const TcpConnectionPtr &conn);
    void removeConn(const string &peerAddr);

    void addTask(const Function &func);
    void addTimer(const Timer &timer) { timerQueue_.addTimer(timer); }
    void removeTimer(const Timer &timer) { timerQueue_.removeTimer(timer); }
    void run();
};


#endif //WEBSERVER_EVENTLOOP_H
