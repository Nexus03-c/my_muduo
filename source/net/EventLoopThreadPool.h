#ifndef MY_MUDUO_EVENTLOOPTHREADPOOL_H
#define MY_MUDUO_EVENTLOOPTHREADPOOL_H
#include "source/base/nonecopyable.h"
#include "EventLoopThread.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>

class EventLoop;

class EventLoopThreadPool : public nonecopyable {
public:
    using ThreadInitFunc = std::function<void(EventLoop*)>;
    EventLoopThreadPool(EventLoop* base_loop, const std::string &name = std::string());
    ~EventLoopThreadPool();

    void start(const ThreadInitFunc &func = ThreadInitFunc());
    void set_thread_nums(int thread_nums) { thread_nums_ = thread_nums; }

    EventLoop* GetNextLoop();
    std::vector<EventLoop*> GetAllLoops();

    bool started() const { return started_;}
    int thread_nums() const { return thread_nums_; }
    const std::string& name() const { return name_; }
private:
    bool started_;
    int thread_nums_;
    int next;
    std::string name_;
    EventLoop* base_loop_;
    std::vector<std::unique_ptr<EventLoopThread>> thread_pool_;
    std::vector<EventLoop*> loops_;
};
#endif