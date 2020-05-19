cmake_minimum_required(VERSION 3.10)
project(WebServer)

set(CMAKE_CXX_STANDARD 11)


add_executable(WebServer main.cpp Logging.cpp Logging.h EventLoop.cpp EventLoop.h Util.cpp Util.h TcpServer.cpp TcpServer.h TcpConnection.cpp TcpConnection.h Mutex.h TaskQueue.h HttpRequest.h Timer.h TimerQueue.cpp TimerQueue.h Timer.cpp TaskQueue.cpp Thread.cpp Thread.h Channel.cpp Channel.h AsyncLogging.cpp AsyncLogging.h Buffer.h noncopyable.h FileUtil.cpp FileUtil.h LogFile.cpp LogFile.h)

find_package(Threads)

target_link_libraries(WebServer ${CMAKE_THREAD_LIBS_INIT})