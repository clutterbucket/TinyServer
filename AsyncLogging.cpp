//
// Created by WTF on 2020/5/8.
//

#include "AsyncLogging.h"
#include <cassert>

void *threadwork(void *var){
    AsyncLogging *log = static_cast<AsyncLogging *>(var);
    log->threadFun();
}

AsyncLogging::AsyncLogging():
    currentBuffer_(new LargeBuffer),
    nextBuffer_(new LargeBuffer),
    cond_(mutex_),
    latch_(1),
    flushInterval_(2)
{
    pthread_t pid;
    if (pthread_create(&pid, NULL, threadwork, (void*)this) == -1){
        LOG_ERR;
    }
    start();
}

void AsyncLogging::threadFun() {
    latch_.countDown();
    LogFile output(logFile_);
    LargeBufferPtr newBuffer1(new LargeBuffer);
    LargeBufferPtr newBuffer2(new LargeBuffer);
//    newBuffer1->bzero();
//    newBuffer2->bzero();
    BufferVec buffersToWrite;
    buffersToWrite.reserve(16);
    while(running){
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());
        {
            MutexLockGuard lock(mutex_);
            if (buffers_.empty())  // unusual usage!
            {
                cond_.waitForSeconds(flushInterval_);
            }
            buffers_.push_back(currentBuffer_);
            currentBuffer_.reset();

            currentBuffer_ = std::move(newBuffer1);
            buffersToWrite.swap(buffers_);
            if (!nextBuffer_) {
                nextBuffer_ = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if (buffersToWrite.size() > 25) {
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        for (size_t i = 0; i < buffersToWrite.size(); ++i) {
            // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            output.append(buffersToWrite[i]->data(), buffersToWrite[i]->length());
        }

        if (buffersToWrite.size() > 2) {
            // drop non-bzero-ed buffers, avoid trashing
            buffersToWrite.resize(2);
        }

        if (!newBuffer1) {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2) {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.back();
            buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
}

void AsyncLogging::append(const char *data, int len) {
    MutexLockGuard lock(mutex_);
    if (currentBuffer_->avail() > len)
        currentBuffer_->append(data, len);
    else {
        buffers_.push_back(currentBuffer_);
        currentBuffer_.reset();
        if (nextBuffer_)
            currentBuffer_ = std::move(nextBuffer_);
        else
            currentBuffer_.reset(new LargeBuffer);
        currentBuffer_->append(data, len);
        cond_.notify();
    }
}
