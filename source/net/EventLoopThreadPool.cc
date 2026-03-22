#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* base_loop, const std::string &name) 
  : started_(false),
    base_loop_(base_loop),
    name_(name),
    next(0),
    thread_nums_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool() {

}

void EventLoopThreadPool::start(const ThreadInitFunc &func) {
    started_ = true;
    for(int i = 0; i<thread_nums_; i++) {
        char buf[name_.size()+32];
        snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
        EventLoopThread* loop_thread = new EventLoopThread(func, buf);
        thread_pool_.push_back(std::unique_ptr<EventLoopThread>(loop_thread));
        loops_.push_back(loop_thread->StartLoop());
    }
    if(thread_nums_ == 0 && func) {
        func(base_loop_);
    }
}

EventLoop* EventLoopThreadPool::GetNextLoop() {
    EventLoop* loop = base_loop_;
    if(!loops_.empty()) {
        loop = loops_[next];
        next++;
        if(static_cast<size_t>(next) >= loops_.size()) {
            next = 0;
        }
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::GetAllLoops() {
    if(loops_.empty()) {
        return std::vector<EventLoop*>(1, base_loop_);
    }else {
        return loops_;
    }
}