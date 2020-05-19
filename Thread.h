//
// Created by WTF on 2020/5/6.
//

#ifndef WEBSERVER_THREAD_H
#define WEBSERVER_THREAD_H


#include "EventLoop.h"

class Thread {
    EventLoop *loop_;
public:
    Thread();
    EventLoop *getLoop();
};


#endif //WEBSERVER_THREAD_H
