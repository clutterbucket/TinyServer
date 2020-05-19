//
// Created by WTF on 2020/4/30.
//

#include "Logging.h"
#include "AsyncLogging.h"

AsyncLogging *asyncLogging = new AsyncLogging;

Logging::~Logging() {
    *this << '\n';
    if (level > 0) {
        abort();
    }
}

void Logging::setLogFile(const char *filename) {
    asyncLogging->setLogFile(filename);
}
