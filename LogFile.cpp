//
// Created by WTF on 2020/5/10.
//

#include "LogFile.h"

using namespace std;

LogFile::LogFile(const char *basename, int flushEveryN)
        : basename_(basename),
          flushEveryN_(flushEveryN),
          count_(0),
          mutex_(new MutexLock) {
    // assert(basename.find('/') >= 0);
    file_.reset(new AppendFile(basename));
}

void LogFile::append(const char* logline, int len) {
    MutexLockGuard lock(*mutex_);
    append_unlocked(logline, len);
}

void LogFile::flush() {
    MutexLockGuard lock(*mutex_);
    file_->flush();
}

void LogFile::append_unlocked(const char* logline, int len) {
    file_->append(logline, len);
    ++count_;
    if (count_ >= flushEveryN_) {
        count_ = 0;
        file_->flush();
    }
}
