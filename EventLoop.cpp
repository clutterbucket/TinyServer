//
// Created by WTF on 2020/4/30.
//

#include "EventLoop.h"
#include "Logging.h"
#include "TcpConnection.h"
#include <sys/epoll.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses"


EventLoop::EventLoop():
    epfd_(epoll_create(1024)),
    taskQueue_(this),
    timerQueue_(this),
    timeOut_(false),
    taskCome_(false)
    {}

void EventLoop::addChannel(const Channel &channel) {
    epoll_event event;
    event.events = channel.getEvents();
    event.data.ptr = const_cast<Channel *>(&channel);
    int ret = epoll_ctl(epfd_, EPOLL_CTL_ADD, channel.getFd(), &event);
    if(ret < 0){
        LOG_ERR;
    }
}

void EventLoop::removeChannel(const Channel &channel) {
    int ret = epoll_ctl(epfd_, EPOLL_CTL_DEL, channel.getFd(), NULL);
    if(ret < 0){
        LOG_ERR;
    }
}

void EventLoop::updateChannel(const Channel &channel) {
    epoll_event event;
    event.events = channel.getEvents();
    event.data.ptr = const_cast<Channel *>(&channel);
    int ret = epoll_ctl(epfd_, EPOLL_CTL_MOD, channel.getFd(), &event);
    if(ret < 0){
        LOG_ERR;
    }
}

void EventLoop::run(){
    while(1){
//        LOG << "Loop";
        epoll_event evts[1024];
        int n = epoll_wait(epfd_, evts, 1024, -1);
        if(n < 0) {
            LOG_ERR;
            return;
        }
        for (int i = 0; i < n; ++i) {
            Channel *channel = (Channel *)evts[i].data.ptr;
            if(channel->getFd() == timerQueue_.getFd()){
                timeOut_ = true;
                continue;
            }
            if(channel->getFd() == taskQueue_.getFd()){
                taskCome_ = true;
                continue;
            }
            channel->handleEvents(evts[i].events);
        }
        if(timeOut_){
            timerQueue_.handleTimeOut();
            timeOut_ = false;
        }
        if(taskCome_){
            taskQueue_.handleTasks();
            taskCome_ = false;
        }
    }
}

void EventLoop::addTask(const Function &func) {
    taskQueue_.put(func);
    uint64_t one = 1;
    ssize_t n = write(taskQueue_.getFd(), &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERR;
    }
}

void EventLoop::establishConn(const string &peerAddr, const TcpConnectionPtr &conn) {
    addChannel(conn->getChannel());
    connections_[peerAddr] = conn;
    conn->getTimer().setConnection(conn);
    conn->getTimer().setHandleFunc(std::bind(&TcpConnection::handleKeepAliveTimeOut, conn->getTimer()));
    timerQueue_.addTimer(conn->getTimer());
}

void EventLoop::removeConn(const string &peerAddr) {
    LOG << peerAddr << " down";
    auto it = connections_.find(peerAddr);
    if(it == connections_.end()){
        return;
    }
    removeChannel(it->second->getChannel());
    connections_.erase(peerAddr);
}

#pragma clang diagnostic pop