//
// Created by WTF on 2020/5/6.
//

#ifndef WEBSERVER_TIMER_H
#define WEBSERVER_TIMER_H


#include <cstdint>
#include <memory>
#include "Util.h"

class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::weak_ptr<TcpConnection> wTcpConnectionPtr;

class Timer {
    friend class TimerQueue;
    int64_t timeOut_;
    Function timeOutHandler_;
    int64_t timerSeq_;
    wTcpConnectionPtr wConn_;
    static int64_t seq_;
public:
    int getSeq() { return timerSeq_; }


    Timer(int64_t timeOut):
        timeOut_(timeOut),
        timerSeq_(seq_++)
        {}

    void setHandleFunc(const Function &func){ timeOutHandler_ = func; }
    void setTimeOut(int64_t timeout) { timeOut_ = timeout; }
    void setConnection(const wTcpConnectionPtr &wConn) { wConn_ = wConn; }
    const wTcpConnectionPtr &getConnection() const { return wConn_;}
    void handle() const { timeOutHandler_(); }

    bool operator<(const Timer &rhs) const {
        if(timeOut_ != rhs.timeOut_){
            return timeOut_ < rhs.timeOut_;
        }
        return timerSeq_ < rhs.timerSeq_;
    }
    bool operator==(const Timer &rhs) const {
        return timerSeq_ == rhs.timerSeq_;
    }
};


#endif //WEBSERVER_TIMER_H
