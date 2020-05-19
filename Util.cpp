//
// Created by WTF on 2020/4/30.
//

#include "Util.h"
#include "Logging.h"
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int64_t now() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return seconds * 1000 * 1000 + tv.tv_usec;
}

int createTimerfd() {
    int timerfd = timerfd_create(CLOCK_MONOTONIC,
                                   TFD_NONBLOCK | TFD_CLOEXEC);
    if (timerfd < 0)
    {
        LOG_ERR;
    }
    return timerfd;
}

int createEventfd() {
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_ERR;
    }
    return evtfd;
}

int createSocketfd() {
    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(fd < 0){
        LOG_ERR;
    }
    return fd;
}
