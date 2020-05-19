//
// Created by WTF on 2020/4/30.
//

#ifndef WEBSERVER_UTIL_H
#define WEBSERVER_UTIL_H

#define KEEPALIVE (2000 * 1000)

#include <functional>

typedef struct sockaddr SA;
typedef std::function<void()> Function;
int64_t now();

int createTimerfd();
int createEventfd();
int createSocketfd();

#endif //WEBSERVER_UTIL_H
