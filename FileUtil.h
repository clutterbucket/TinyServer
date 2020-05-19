//
// Created by WTF on 2020/5/10.
//

#ifndef WEBSERVER_FILEUTIL_H
#define WEBSERVER_FILEUTIL_H

#include <string>
#include "noncopyable.h"


class AppendFile : noncopyable {
public:
    explicit AppendFile(const char *filename);
    ~AppendFile();
    // append 会向文件写
    void append(const char *logline, const size_t len);
    void flush();

private:
    size_t write(const char *logline, size_t len);
    FILE *fp_;
    char buffer_[64 * 1024];
};


#endif //WEBSERVER_FILEUTIL_H
