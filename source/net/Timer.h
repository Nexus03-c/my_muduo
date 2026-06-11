#ifndef MY_MUDUO_TIMER_H
#define MY_MUDUO_TIMER_H
#include "source/base/nonecopyable.h"
#include "Callbacks.h"
#include "source/base/TimeStamp.h"

#include <atomic>

class Timer : public nonecopyable {
public:
    Timer(TimerCallback cb, TimeStamp when, double interval)
      : callback_(std::move(cb)),
        expiration_(when),
        interval_(interval),
        repeat_(interval_>0),
        sequence_(++num_created_)
    {}
    
    void run() {
        callback_();
    }

    bool repeat() { return repeat_; }
    int64_t sequence() { return sequence_; }
    TimeStamp expiration() { return expiration_; }

    void restart(TimeStamp now);
private:
    const TimerCallback callback_;
    TimeStamp expiration_;
    const double interval_;
    const double repeat_;
    const int64_t sequence_;

    static std::atomic<int64_t> num_created_;
};

#endif