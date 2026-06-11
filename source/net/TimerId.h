#ifndef MY_MUDUO_TIMERID_H
#define MY_MUDUO_TIMERID_H
#include <stdint.h>
class Timer;

class TimerId {
public:
    TimerId(): timer_(nullptr), sequence_(0){}

    TimerId(Timer* timer, int64_t sequence): timer_(timer), sequence_(sequence) {}
    friend class TimerQueue;
private:
    Timer* timer_;
    int64_t sequence_;
};


#endif