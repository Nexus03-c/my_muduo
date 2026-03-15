#ifndef MY_MUDUO_EVENTLOOPTHREAD_H
#define MY_MUDUO_EVENTLOOPTHREAD_H
#include "source/base/Thread.h"
#include <mutex>
#include <condition_variable>
#include <functional>
#include <string>

class EventLoop;

class EventLoopThread : public nonecopyable {
public:
    using ThreadInitFunc = std::function<void(EventLoop*)>;

    explicit EventLoopThread(const ThreadInitFunc &cb = ThreadInitFunc(), const std::string& name = std::string());
    ~EventLoopThread();

    EventLoop* StartLoop();
private:
    void ThreadFunc();
    bool exit_;
    std::mutex mutex_;
    std::condition_variable cond_;
    EventLoop *loop_;
    Thread thread_;
    ThreadInitFunc cb_;
};

#endif