#include <errno.h>
#include <error.h>
#include "LogFile.h"

LogFile::LogFile(std::string file_name):
    log_file_(file_name),
    fp_(fopen(file_name.c_str(), "ae")) 
{

}

LogFile::~LogFile() {
    fclose(fp_);
}

void LogFile::append(const char* str, int len) {
    int remain = len;
    while(remain > 0) {
        int bytes = ::fwrite_unlocked(str, remain, 1, fp_);
        if(bytes != remain) {
            int err = ::ferror(fp_);
            if(err) {
                fprintf(stderr, "LogFile::append() failed %d", err);
                break;
            }
        }
        remain -= bytes;
    }
}

void LogFile::flush() {
    ::fflush(fp_);
}