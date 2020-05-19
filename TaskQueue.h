//
// Created by WTF on 2020/5/3.
//

#ifndef WEBSERVER_TASKQUEUE_H
#define WEBSERVER_TASKQUEUE_H

#include "Mutex.h"
#include "Util.h"
#include "Logging.h"
#include "Channel.h"
#include <deque>
#include <sys/eventfd.h>

class EventLoop;
class TaskQueue{
    MutexLock mutex_;
    std::deque<Function> taskQueue_;
    Channel channel_;
    EventLoop *loop_;
public:
    TaskQueue(EventLoop *loop);
    void put(const Function &func);
    Function take();
    int getFd(){ return channel_.getFd(); }
    void handleTasks();
    int getSize(){ return taskQueue_.size(); }
};

#endif //WEBSERVER_TASKQUEUE_H
