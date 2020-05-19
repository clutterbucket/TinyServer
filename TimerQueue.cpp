//
// Created by WTF on 2020/5/6.
//

#include "TimerQueue.h"
#include "Util.h"
#include "EventLoop.h"
#include <sys/timerfd.h>

TimerQueue::TimerQueue(EventLoop *loop) :
    channel_(createTimerfd()),
    loop_(loop)
{
    channel_.enablePollIn();
    loop_->addChannel(channel_);
}

void TimerQueue::handleTimeOut() {
    int64_t howmany;
    int ret = read(channel_.getFd(), &howmany, sizeof(howmany));
    if(ret != sizeof howmany){
        LOG_ERR;
    }
    int64_t timeNow = now();
    auto it = timerQueue_.begin();
    while(it != timerQueue_.end() && it->timeOut_ <= timeNow){
        it->handle();
        ++it;
    }
    timerQueue_.erase(timerQueue_.begin(), it);
    if(!timerQueue_.empty()){
        resetTimer();
    }
}

void TimerQueue::addTimer(const Timer &timer) {
    bool needResetTimer = false;
    if(timerQueue_.empty()){
        needResetTimer = true;
    }
    else{
        auto it = timerQueue_.begin();
        if(timer.timeOut_ < it->timeOut_){
            needResetTimer = true;
        }
    }
    timerQueue_.insert(timer);
    if(needResetTimer){
        resetTimer();
    }
}

void TimerQueue::removeTimer(const Timer &timer) {
    if(timerQueue_.empty()){
        LOG_ERR;
    }
    auto it = timerQueue_.begin();
    bool needResetTimer = false;
    if(timer == *it){
        needResetTimer = true;
    }
    timerQueue_.erase(timer);
    if(needResetTimer && !timerQueue_.empty()){
        resetTimer();
    }
}

void TimerQueue::resetTimer() {
    struct itimerspec newValue = {};
    struct itimerspec oldValue = {};
    struct timespec ts;
    assert(!timerQueue_.empty());
    auto it = timerQueue_.begin();
    int64_t timeout = it->timeOut_ - now();
    timeout = (timeout < 100) ? 100 : timeout;
    ts.tv_sec = timeout / (1000 * 1000);
    ts.tv_nsec = (timeout % (1000 * 1000) * 1000);
    newValue.it_value = ts;
    int ret = timerfd_settime(channel_.getFd(), 0, &newValue, &oldValue);
    if (ret)
    {
        LOG_ERR;
    }
}
