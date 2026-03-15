#include "Thread.h"
#include "CurrentThrea.h"
#include <thread>
#include <semaphore.h>

std::atomic<int> Thread::thread_nums_ = 0;

Thread::Thread(ThreadFunc func, const std::string &name = std::string()) 
  : started_(false),
    joined_(false),
    tid_(0),
    func_(std::move(func)),
    name_(name)
{
    SetDefaultName();
}

Thread::~Thread() {
    if(started_ && !joined_) {
        thread_->detach();
    }
}

void Thread::start() {
    started_ = true;
    sem_t sem;
    sem_init(&sem, 0, 0);
    thread_ = std::unique_ptr<std::thread>(new std::thread([&]() {
        tid_ = tid();
        sem_post(&sem);
        func_();
    }));

    sem_wait(&sem);
}

void Thread::join() {
    joined_ = true;
    thread_->join();
}

void Thread::SetDefaultName() {
    int num = ++thread_nums_;
    if(name_.empty()) {
        char buf[64];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    } 
}