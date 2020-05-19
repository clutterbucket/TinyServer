//
// Created by WTF on 2020/5/7.
//

#ifndef TINYSERVER_LOGGING_H
#define TINYSERVER_LOGGING_H

#include <iostream>
#include <cstring>
#include "Buffer.h"

class AsyncLogging;

class Logging {
    int level;
    Buffer<kSmallBuffer> buffer_;
public:
    explicit Logging(int _level) : level(_level){}
    ~Logging();

    Logging &operator<<(bool v){
        buffer_.append(v ? "1": "0", 1);
        return *this;
    }

    Logging &operator<<(char ch){
        char s[2] = {};
        s[0] = ch;
        buffer_.append(s, 1);
        return *this;
    }

    Logging &operator<<(const char *s){
        buffer_.append(s, strlen(s));
        return *this;
    }

    Logging &operator<<(char *s){
        buffer_.append(s, strlen(s));
        return *this;
    }

    Logging &operator<<(const std::string &s){
        buffer_.append(s.c_str(), s.size());
        return *this;
    }

    template <typename T>
    Logging &operator<<(T num){
        std::string s = std::to_string(num);
        buffer_.append(s.c_str(), s.size());
        return *this;
    }

    static void setLogFile(const char *filename);
};

#define LOG Logging(0) << __FILE__  << ":" <<  __LINE__ << ": " << __FUNCTION__ << "(): "
#define LOG_ERR Logging(1) << __FILE__  << ":" <<  __LINE__ << ": " << __FUNCTION__ << "(): " << strerror(errno)
#define LOG_FATAL Logging(2) << __FILE__  << ":" <<  __LINE__ << ": " << __FUNCTION__ << "(): "

#endif //TINYSERVER_LOGGING_H
