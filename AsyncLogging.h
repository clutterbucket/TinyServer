//
// Created by WTF on 2020/5/8.
//

#ifndef WEBSERVER_ASYNCLOGGING_H
#define WEBSERVER_ASYNCLOGGING_H

#include "Buffer.h"
#include "LogFile.h"
#include "Mutex.h"
#include <memory>
#include <vector>
#include "Thread.h"

class AsyncLogging {
    typedef Buffer<kLargeBuffer> LargeBuffer;
    typedef std::shared_ptr<LargeBuffer> LargeBufferPtr;
    typedef std::vector<LargeBufferPtr> BufferVec;

    LargeBufferPtr currentBuffer_;
    LargeBufferPtr nextBuffer_;
    BufferVec buffers_;
    MutexLock mutex_;
    Condition cond_;
    CountDownLatch latch_;
    const char *logFile_ = "../Log.txt";
    bool running = true;
    const int flushInterval_;

public:
    AsyncLogging();
    void setLogFile(const char *filename) { logFile_ = filename; }
    void threadFun();
    void start(){

    }
    void append(const char *data, int len);
};


#endif //WEBSERVER_ASYNCLOGGING_H
