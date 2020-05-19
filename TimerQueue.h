//
// Created by WTF on 2020/5/6.
//

#ifndef WEBSERVER_TIMERQUEUE_H
#define WEBSERVER_TIMERQUEUE_H

#include <sys/timerfd.h>
#include <cstring>
#include <set>
#include <cassert>
#include "Logging.h"
#include "Timer.h"
#include "Channel.h"

class EventLoop;

class TimerQueue {
    Channel channel_;
    std::set<Timer> timerQueue_;
    EventLoop *loop_;
public:
    bool test() {
        for(auto timer: timerQueue_){
            LOG << timer.timerSeq_;
            if(!timer.getConnection().lock()){
                return false;
            }
        }
        return true;
    }
    TimerQueue(EventLoop *loop);
    void handleTimeOut();
    void addTimer(const Timer &timer);
    void removeTimer(const Timer &timer);
    int getFd() { return channel_.getFd(); }
    int getSize() { return timerQueue_.size(); }
    void resetTimer();
};


#endif //WEBSERVER_TIMERQUEUE_H
