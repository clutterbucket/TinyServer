//
// Created by WTF on 2020/5/10.
//

#ifndef WEBSERVER_LOGFILE_H
#define WEBSERVER_LOGFILE_H

#include "noncopyable.h"
#include <string>
#include <memory>
#include "Mutex.h"
#include "FileUtil.h"

class LogFile : noncopyable {
public:
    // 每被append flushEveryN次，flush一下，会往文件写，只不过，文件也是带缓冲区的
    LogFile(const char *basename, int flushEveryN = 1024);

    void append(const char* logline, int len);
    void flush();

private:
    void append_unlocked(const char* logline, int len);

    const std::string basename_;
    const int flushEveryN_;

    int count_;
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;
};


#endif //WEBSERVER_LOGFILE_H
