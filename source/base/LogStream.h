#ifndef MY_MUDUO_LOGSTREAM_H
#define MY_MUDUO_LOGSTREAM_H
#include <string.h>

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
private:
    const char* end() { return buffer_ + Size;}
    char buffer_[Size];
    char *cur_;
};
#endif