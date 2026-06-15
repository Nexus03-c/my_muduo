#ifndef MY_MUDUO_TIMESTAMP_H
#define MY_MUDUO_TIMESTAMP_H

#include <iostream>
#include <string>

class TimeStamp {
public:
    static const int kMicroSecondsPerSecond = 1000 * 1000;
    TimeStamp() :microsecondsSinceEpoch_(0) {};
    explicit TimeStamp(int64_t microsecondsSinceEpoch) :microsecondsSinceEpoch_(microsecondsSinceEpoch) {};
    ~TimeStamp() = default;
    static TimeStamp now();
    std::string toString();
    int64_t microsecondsSinceEpoch() { return microsecondsSinceEpoch_; }

    static TimeStamp invalid() { return TimeStamp(); }
    bool valid() const { return microsecondsSinceEpoch_ > 0; }
    void swap(TimeStamp& time) { std::swap(microsecondsSinceEpoch_, time.microsecondsSinceEpoch_); }
private:
    int64_t microsecondsSinceEpoch_;
};

inline bool operator<(TimeStamp lhs, TimeStamp rhs) {
    return lhs.microsecondsSinceEpoch() < rhs.microsecondsSinceEpoch();
}

inline bool operator==(TimeStamp lhs, TimeStamp rhs) {
    return lhs.microsecondsSinceEpoch() == rhs.microsecondsSinceEpoch();
}

inline TimeStamp addTime(TimeStamp now, double seconds) {
    int64_t detla = static_cast<int64_t>(seconds * TimeStamp::kMicroSecondsPerSecond);
    return TimeStamp(now.microsecondsSinceEpoch() + detla);
}
#endif