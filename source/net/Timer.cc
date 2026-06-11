#include "Timer.h"

std::atomic<int64_t> Timer::num_created_;

void Timer::restart(TimeStamp now) {
    if(repeat_) {
        expiration_ = addTime(now, interval_);
    } else {
        expiration_ = TimeStamp::invalid();
    }
}