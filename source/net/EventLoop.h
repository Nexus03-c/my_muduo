#ifndef MY_MUDUO_EVENTLOOP_H
#define MY_MUDUO_EVENTLOOP_H

#include "source/base/nonecopyable.h"
#include "source/base/TimeStamp.h"
#include "source/base/CurrentThrea.h"
#include <atomic>
#include <functional>
#include <memory>
#include <vector>
#include <mutex>

class Poller;
class Channel;

class EventLoop : public nonecopyable {
public:
    using Functor = std::function<void()>;
    
    EventLoop();
    ~EventLoop();

    void Loop();
    void Quit();

    TimeStamp poller_return_time() { return poller_return_time_; }

    void RunInLoop(Functor cb);
    void QueueInLoop(Functor cb);

    //wake up thread through event fd
    void WakeUp();
    //update channel to poller
    void UpdateChannel(Channel* channel);
    void RemoveChannel(Channel* channel);
    bool HasChannel(Channel* channel);

    //now thread is in loop thread?
    bool IsInLoopThread() { return thread_id_ == tid(); }

private:
    //wake up callback
    void HandleRead();
    //execute pending functor
    void DoPendingFunctors();

    using ChannelList = std::vector<Channel*>;
    std::atomic_bool looping_;
    std::atomic_bool quit_;
    std::atomic_bool callingPendingFunctors;
    const pid_t thread_id_;
    TimeStamp poller_return_time_;

    std::unique_ptr<Poller> poller_;
    int wakeup_fd_;
    std::unique_ptr<Channel> wakeup_channel_;

    ChannelList active_channels_;
    Channel* current_active_channel_;

    std::mutex mutex_;
    std::vector<Functor> pending_functors_;
};

#endif