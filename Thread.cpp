//
// Created by WTF on 2020/5/6.
//

#include "Thread.h"
#include "EventLoop.h"
#include "Logging.h"

void *thread_work(void *arg){
    LOG << "start working";
    EventLoop *loop = (EventLoop *)arg;
    loop->run();
}

Thread::Thread():
    loop_(new EventLoop)
{
    pthread_t pid;
    if (pthread_create(&pid, NULL, thread_work, (void*)loop_) == -1){
        LOG_ERR;
    }
}

EventLoop *Thread::getLoop() {
    return loop_;
}
