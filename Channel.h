//
// Created by WTF on 2020/4/30.
//

#ifndef WEBSERVER_SOCKET_H
#define WEBSERVER_SOCKET_H

#include <functional>
#include <sys/epoll.h>
#include <unistd.h>
#include "Util.h"

class Channel {
    int fd_;
    uint32_t events_;
    Function handlePollIn_;
    Function handlePollOut_;
    Function handlePollErr_;
public:
    explicit Channel(int fd) : fd_(fd), events_(0) {}
    ~Channel() { close(fd_); }
    void setPollInFunc(const Function &handlePollIn){ handlePollIn_ = handlePollIn; }
    void setPollOutFunc(const Function &handlePollOut){ handlePollOut_ = handlePollOut; }
    void setPollErrFunc(const Function &handlePollErr){ handlePollErr_ = handlePollErr; }
    int getFd() const { return fd_; }
    uint32_t getEvents() const { return events_; }
    void enablePollIn() { events_ |= EPOLLIN; }
    void enablePollOut() { events_ |= EPOLLOUT; }
    void disablePollIn() { events_ &= ~EPOLLIN; }
    void disablePollOut(){ events_ &= ~EPOLLOUT; }
    bool pollOutEnabled() { return events_ & EPOLLOUT; }
    void handleEvents(uint32_t occuredEvents);
};

#endif //WEBSERVER_SOCKET_H
