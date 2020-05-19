//
// Created by WTF on 2020/5/7.
//

#include "Channel.h"

void Channel::handleEvents(uint32_t occuredEvents) {
    if(occuredEvents & EPOLLIN){
        handlePollIn_();
    }
    if(occuredEvents & EPOLLOUT){
        handlePollOut_();
    }
    if(occuredEvents & EPOLLERR){
//        handlePollErr();
    }
}
