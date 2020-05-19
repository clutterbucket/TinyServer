#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-member-init"
#include "EventLoop.h"
#include "TcpServer.h"
#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <functional>
#include <unistd.h>
#include "Logging.h"

using namespace std;

int main(int argc, char *argv[]){
    EventLoop loop;
    TcpServer server("0.0.0.0", "5000", &loop, 1);
    loop.run();
    return 0;
}


#pragma clang diagnostic pop