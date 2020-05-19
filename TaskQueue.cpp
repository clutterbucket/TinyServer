//
// Created by WTF on 2020/5/6.
//
#include "TaskQueue.h"
#include "EventLoop.h"
#include "Util.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses"

TaskQueue::TaskQueue(EventLoop *loop) :
    channel_(createEventfd()),
    loop_(loop)
{
    channel_.enablePollIn();
    loop->addChannel(channel_);
}

Function TaskQueue::take() {
    MutexLockGuard lock(mutex_);
    if(taskQueue_.empty()){
        return Function();
    }
    Function func = taskQueue_.front();
    taskQueue_.pop_front();
    return func;
}

void TaskQueue::put(const Function &func) {
    MutexLockGuard lock(mutex_);
    taskQueue_.push_back(std::move(func));
}

void TaskQueue::handleTasks() {
    uint64_t one = 1;
    ssize_t n = read(channel_.getFd(), &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERR;
    }

    Function func;
    while(func = take()){
        func();
    }
}

#pragma clang diagnostic pop