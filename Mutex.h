//
// Created by WTF on 2020/5/3.
//

#ifndef WEBSERVER_MUTEX_H
#define WEBSERVER_MUTEX_H

#include <pthread.h>
#include <errno.h>
#include "noncopyable.h"

class MutexLock : noncopyable {
public:
    MutexLock() { pthread_mutex_init(&mutex, NULL); }
    ~MutexLock() {
        pthread_mutex_lock(&mutex);
        pthread_mutex_destroy(&mutex);
    }
    void lock() { pthread_mutex_lock(&mutex); }
    void unlock() { pthread_mutex_unlock(&mutex); }
    pthread_mutex_t *get() { return &mutex; }

private:
    pthread_mutex_t mutex;

    friend class Condition;
};

class MutexLockGuard : noncopyable {
public:
    explicit MutexLockGuard(MutexLock &_mutex) : mutex(_mutex) { mutex.lock(); }
    ~MutexLockGuard() { mutex.unlock(); }

private:
    MutexLock &mutex;
};

class Condition : noncopyable {
public:
    explicit Condition(MutexLock &_mutex) : mutex(_mutex) {
        pthread_cond_init(&cond, NULL);
    }
    ~Condition() { pthread_cond_destroy(&cond); }
    void wait() { pthread_cond_wait(&cond, mutex.get()); }
    void notify() { pthread_cond_signal(&cond); }
    void notifyAll() { pthread_cond_broadcast(&cond); }
    bool waitForSeconds(int seconds) {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&cond, mutex.get(), &abstime);
    }

private:
    MutexLock &mutex;
    pthread_cond_t cond;
};


class CountDownLatch {
public:
    explicit CountDownLatch(int count) : mutex_(), condition_(mutex_), count_(count) {}
    void wait(){
        MutexLockGuard lock(mutex_);
        while (count_ > 0) condition_.wait();
    }
    void countDown(){
        MutexLockGuard lock(mutex_);
        --count_;
        if (count_ == 0) condition_.notifyAll();
    }

private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};



#endif //WEBSERVER_MUTEX_H
