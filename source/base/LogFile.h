#ifndef MY_MUDUO_LOGFILE_H
#define MY_MUDUO_LOGFILE_H
#include <string>
#include <stdio.h>
#include "nonecopyable.h"

class LogFile : public nonecopyable {
public:
    LogFile(std::string file_name);
    ~LogFile();

    void append(const char* str, int len);
    void flush();
private:
    std::string log_file_;
    FILE* fp_;
};
#endif