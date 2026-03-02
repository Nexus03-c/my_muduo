#include <stdio.h>
#include "LogStream.h"

LogStream& LogStream::operator<<(short num) {
    *this<<static_cast<int>(num);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short num) {
    *this<<static_cast<int>(num);
    return *this;
}

LogStream& LogStream::operator<<(int num) {
    if(buffer_.avail() >= LogStream::kMaxNumericSize) {
        char* buffer = buffer_.current();
        snprintf(buffer, buffer_.avail(), "%d", num);
    }
    return *this;
}

LogStream& LogStream::operator<<(unsigned int num) {
    if(buffer_.avail() >= LogStream::kMaxNumericSize) {
        char* buffer = buffer_.current();
        snprintf(buffer, buffer_.avail(), "%d", num);
    }
    return *this;
}

LogStream& LogStream::operator<<(long num) {
    if(buffer_.avail() >= LogStream::kMaxNumericSize) {
        char* buffer = buffer_.current();
        snprintf(buffer, buffer_.avail(), "%ld", num);
    }
    return *this;
}

LogStream& LogStream::operator<<(unsigned long num) {
    if(buffer_.avail() >= LogStream::kMaxNumericSize) {
        char* buffer = buffer_.current();
        snprintf(buffer, buffer_.avail(), "%ld", num);
    }
    return *this;
}

LogStream& LogStream::operator<<(long long num) {
    if(buffer_.avail() >= LogStream::kMaxNumericSize) {
        char* buffer = buffer_.current();
        snprintf(buffer, buffer_.avail(), "%lld", num);
    }
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long num) {
    if(buffer_.avail() >= LogStream::kMaxNumericSize) {
        char* buffer = buffer_.current();
        snprintf(buffer, buffer_.avail(), "%lld", num);
    }
    return *this;
}

LogStream& LogStream::operator<<(float num){
    *this<<static_cast<double>(num);
    return *this;
}

LogStream& LogStream::operator<<(double num) {
    if(buffer_.avail() >= LogStream::kMaxNumericSize) {
        char* buffer = buffer_.current();
        snprintf(buffer, buffer_.avail(), "%lf", num);
    }
    return *this;
}

LogStream& LogStream::operator<<(char c) {
    buffer_.append(&c, 1);
    return *this;
}

LogStream& LogStream::operator<<(unsigned char c) {
    *this<<static_cast<char>(c);
    return *this;
}

LogStream& LogStream::operator<<(const char *str) {
    buffer_.append(str, strlen(str));
    return *this;
}

LogStream& LogStream::operator<<(const unsigned char *str) {
    *this<<reinterpret_cast<const char*>(str);
    return *this;
}

LogStream& LogStream::operator<<(const std::string &str) {
    buffer_.append(str.c_str(), str.size());
    return *this;
}