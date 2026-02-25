#ifndef MY_MUDUO_TIMESTAMP_H
#define MY_MUDUO_TIMESTAMP_H

#include <iostream>
#include <string>

class TimeStamp {
public:
    TimeStamp() :microsecondsSinceEpoch_(0) {};
    explicit TimeStamp(int64_t microsecondsSinceEpoch) :microsecondsSinceEpoch_(microsecondsSinceEpoch) {};
    ~TimeStamp() = default;
    static TimeStamp now();
    std::string toString();
private:
    int64_t microsecondsSinceEpoch_;
};
#endif