#ifndef MY_MUDUO_TIMERQUEUE_H
#define MY_MUDUO_TIMERQUEUE_H
#include "source/base/nonecopyable.h"
#include "Channel.h"
#include "TimerId.h"
#include "Callbacks.h"

#include <set>
#include <vector>
#include <atomic>

class EventLoop;
class Timer;

class TimerQueue : public nonecopyable {
public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId AddTimer(TimerCallback cb, TimeStamp when, double interval);

    void Cancel(TimerId);
private:
    using Entry = std::pair<TimeStamp, Timer*>;
    using TimerList = std::set<Entry>;
    using ActiveTimer = std::pair<Timer*, int64_t>;
    using ActiveTimerSet = std::set<ActiveTimer>;

    void AddTimerInLoop(Timer* timer);
    void CancelInLoop(TimerId timer_id);

    void HandleRead();
    std::vector<Entry> GetExpired(TimeStamp now);
    void Reset(const std::vector<Entry> &expired, TimeStamp now);

    bool insert(Timer* timer);

    EventLoop* loop_;
    const int timer_fd_;
    Channel timer_channel_;

    TimerList timers_;
    ActiveTimerSet activetimer_set_;
    std::atomic_bool calling_expiredtimer_;
    ActiveTimerSet canceling_timers_;
};

#endif