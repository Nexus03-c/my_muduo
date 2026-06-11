#include "TimerQueue.h"
#include "Timer.h"
#include "EventLoop.h"
#include "source/base/Logging.h"
#include <sys/timerfd.h>
#include <functional>
#include <unistd.h>
#include <iterator>

int CreateTimerFd() {
    int timer_fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timer_fd < 0) {
        LOG_FATAL<<"Failed to create timer fd";
    }
    return timer_fd;
}

timespec HowMuchTimeFromNow(TimeStamp when) {
    timespec ts;
    int64_t micro_sec = when.microsecondsSinceEpoch() - TimeStamp::now().microsecondsSinceEpoch();
    ts.tv_sec = static_cast<time_t>(micro_sec/TimeStamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<time_t>(micro_sec%TimeStamp::kMicroSecondsPerSecond * 1000);
    return ts;
}

void ResetTimerFd(int timer_fd, TimeStamp expiration) {
    itimerspec new_time;
    itimerspec old_time;
    bzero(&new_time, sizeof(new_time));
    bzero(&old_time, sizeof(old_time));
    new_time.it_value = HowMuchTimeFromNow(expiration);
    int res = timerfd_settime(timer_fd, 0, &new_time, &old_time);
    if(res<0) {
        LOG_ERROR<<"timerfd_settime failed";
    }
}

void ReadTimerFd(int timer_fd, TimeStamp now) {
    uint64_t how_many = 0;
    ssize_t n = ::read(timer_fd, &how_many, sizeof(how_many));
    LOG_INFO<<"TimerQueue HandleRead "<<how_many<<" at "<<now.toString();
    if(n != sizeof(how_many)) {
        LOG_ERROR<<"TimerQueue HandleRead read "<<n << " bytes instead of 8";
    }
}

TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timer_fd_(CreateTimerFd()),
    timer_channel_(loop, timer_fd_),
    timers_(),
    calling_expiredtimer_(false)
{
    timer_channel_.setReadCallback(std::bind(&TimerQueue::HandleRead, this));
    timer_channel_.enableReading();
}

TimerQueue::~TimerQueue() {
    timer_channel_.disableAll();
    timer_channel_.remove();
    ::close(timer_fd_);

    for(const Entry& it : timers_) {
        delete it.second;
    }
}

TimerId TimerQueue::AddTimer(TimerCallback cb, TimeStamp when, double interval) {
    Timer* timer = new Timer(std::move(cb), when, interval);
    loop_->RunInLoop(std::bind(&TimerQueue::AddTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

void TimerQueue::Cancel(TimerId timer_id) {
    loop_->RunInLoop(std::bind(&TimerQueue::CancelInLoop, this, timer_id));
}

void TimerQueue::AddTimerInLoop(Timer* timer) {
    bool earliest_change = insert(timer);

    if(earliest_change) {
        ResetTimerFd(timer_fd_, timer->expiration());
    }
}
void TimerQueue::CancelInLoop(TimerId timer_id) {
    ActiveTimer active_timer(timer_id.timer_, timer_id.sequence_);
    ActiveTimerSet::iterator it = activetimer_set_.find(active_timer);
    
    if(it != activetimer_set_.end()) {
        Entry entry(timer_id.timer_->expiration(), timer_id.timer_);
        timers_.erase(entry);
        activetimer_set_.erase(active_timer);
        delete active_timer.first;
    } else if(calling_expiredtimer_) {
        canceling_timers_.insert(active_timer);
    }
}

void TimerQueue::HandleRead() {
    TimeStamp now = TimeStamp::now();
    ReadTimerFd(timer_fd_, now);

    std::vector<Entry> expired = GetExpired(now);

    calling_expiredtimer_ = true;
    canceling_timers_.clear();
    for(auto &entry : expired) {
        entry.second->run();
    }
    calling_expiredtimer_ = false;
    Reset(expired, now);
}

std::vector<TimerQueue::Entry> TimerQueue::GetExpired(TimeStamp now) {
    std::vector<Entry> expired;
    Entry entry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator end = timers_.lower_bound(entry);
    std::copy(timers_.begin(), end, std::back_inserter(expired));
    timers_.erase(timers_.begin(), end);

    for(const Entry& it : expired) {
        ActiveTimer active_timer(it.second, it.second->sequence());
        activetimer_set_.erase(active_timer);
    }
    return expired;
}

void TimerQueue::Reset(const std::vector<Entry> &expired, TimeStamp now) {
    TimeStamp next_expired;

    for(const Entry& it : expired) {
        ActiveTimer active_timer(it.second, it.second->sequence());
        if(it.second->repeat() && canceling_timers_.find(active_timer) == canceling_timers_.end()) {
            it.second->restart(now);
            insert(it.second);
        } else {
            delete it.second;
        }
    }

    if(!timers_.empty()) {
        next_expired = timers_.begin()->second->expiration();
    }

    if(next_expired.valid()) {
        ResetTimerFd(timer_fd_, next_expired);
    }
}

bool TimerQueue::insert(Timer* timer) {
    bool earliest_change = false;
    TimeStamp when = timer->expiration();
    TimerList::iterator it = timers_.begin();
    if(it == timers_.end() || when < it->first) {
        earliest_change = true;
    }
    timers_.insert(Entry(when, timer));
    activetimer_set_.insert(ActiveTimer(timer, timer->sequence()));
    return earliest_change;
}