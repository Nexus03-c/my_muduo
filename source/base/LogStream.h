#ifndef MY_MUDUO_LOGSTREAM_H
#define MY_MUDUO_LOGSTREAM_H
#include <string.h>
#include <string>

#include "nonecopyable.h"

const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000*1000;

template<int Size>
class FixedBuffer {
public:
    FixedBuffer():cur_(buffer_) {}
    ~FixedBuffer() {}

    int avail() { return end() - cur_;}
    void append(const char *str, int len) {
        if(len < avail()) {
            memcpy(cur_, str, len);
            cur_ += len;
        }
    }

    void reset() {
        cur_ = buffer_;
    }
    void bZero() {
        memset(buffer_, 0, Size);
    }
    const char* data() const {return buffer_;}
    char* current() {return cur_;};
    int length() const { return cur_ - buffer_;}
    void add(size_t len) { cur_ += len;}
private:
    const char* end() { return buffer_ + Size;}
    char buffer_[Size];
    char *cur_;
};

class LogStream : public nonecopyable {
public:
    typedef FixedBuffer<kSmallBuffer> Buffer;
    typedef LogStream self;

    LogStream(): buffer_() {
        buffer_.bZero();
    }

    self& operator<<(bool);
    self& operator<<(short);
    self& operator<<(unsigned short);
    self& operator<<(int);
    self& operator<<(unsigned int);
    self& operator<<(long);
    self& operator<<(unsigned long);
    self& operator<<(long long);
    self& operator<<(unsigned long long);

    self& operator<<(float);
    self& operator<<(double);

    self& operator<<(char);
    self& operator<<(unsigned char);
    self& operator<<(const char *str);
    self& operator<<(const unsigned char *str);
    self& operator<<(const std::string &str);

    const Buffer& buffer() {return buffer_;}
private:
    Buffer buffer_;
    static const int kMaxNumericSize = 48;
};
#endif