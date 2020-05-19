//
// Created by WTF on 2020/5/8.
//

#ifndef WEBSERVER_BUFFER_H
#define WEBSERVER_BUFFER_H


#include <cstring>

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

template <int SIZE>
class Buffer{
    char buf_[SIZE];
    char *cur_;
public:
    Buffer(): cur_(buf_){}
    int avail(){
        return SIZE - length();
    }
    int length(){
        return cur_ - buf_;
    }
    void append(const char *data, int len){
        if(avail() >= len){
            memcpy(cur_, data, len);
            cur_ += len;
        }
    }
    char *data(){ return buf_; }
    void reset() { cur_ = buf_; }
};


#endif //WEBSERVER_BUFFER_H
