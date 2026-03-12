#include "EventLoop.h"
#include "source/base/Logging.h"
#include "Poller.h"
#include "Channel.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <thread>

thread_local EventLoop *t_loopInThisThread = nullptr;
const int kPollTimeMs = 10000;

int CreateEventFd() {
    int event_fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
    if(event_fd < 0) {
        LOG_ERROR<<"Failed in create event fd";
    }
    return event_fd;
}

EventLoop::EventLoop():
    looping_(false),
    quit_(false),
    callingPendingFunctors(false),
    thread_id_(tid()),
    poller_(Poller::newDefaultPoller(this)),
    wakeup_fd_(CreateEventFd()),
    wakeup_channel_(new Channel(this, wakeup_fd_))
{
    LOG_DEBUG<<"EventLoop create in thread "<<thread_id_;
    if(t_loopInThisThread) {
        LOG_ERROR<<"Another loop exists in thread "<<thread_id_;
    } else {
        t_loopInThisThread = this;
    }
    wakeup_channel_->setReadCallback(std::bind(&EventLoop::HandleRead, this));
    wakeup_channel_->enableReading();
}

EventLoop::~EventLoop() {
    wakeup_channel_->disableAll();
    wakeup_channel_->remove();
    close(wakeup_fd_);
}

void EventLoop::Loop() {
    looping_ = true;
    quit_ = false;
    while(!quit_) {
        active_channels_.clear();
        poller_return_time_ = poller_->poll(kPollTimeMs, &active_channels_);

        for(Channel *channel : active_channels_) {
            current_active_channel_ = channel;
            current_active_channel_->handleEvent(poller_return_time_);
        }
        current_active_channel_ = nullptr;

        DoPendingFunctors();
    }
}

void EventLoop::Quit() {
    quit_ = true;
    if(!IsInLoopThread()) {
        WakeUp();
    }
}

void EventLoop::RunInLoop(Functor cb) {
    if(IsInLoopThread()) {
        cb();
    } else {
        QueueInLoop(std::move(cb));
    }
}
void EventLoop::QueueInLoop(Functor cb) {
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pending_functors_.emplace_back(std::move(cb));
    }

    if(!IsInLoopThread() || callingPendingFunctors) {
        WakeUp();
    }
}

void EventLoop::WakeUp() {
    uint64_t one = 1;
    ssize_t n = write(wakeup_fd_, &one, sizeof one);
    if(n < 0) {
        LOG_ERROR<<"EventLoop::wakeup() writes "<<n<<" instead of 8 bytes";
    }
}

void EventLoop::UpdateChannel(Channel* channel) {
    poller_->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(Channel* channel) {
    poller_->RemoveChannel(channel);
}

bool EventLoop::HasChannel(Channel* channel) {
    return poller_->HasChannel(channel);
}

void EventLoop::HandleRead() {
    uint64_t one;
    ssize_t n = read(wakeup_fd_, &one, sizeof one);
    if(n != sizeof one) {
        LOG_ERROR<<"EventLoop::wakeup() read "<<n<<" instead of 8 bytes";
    }
}

void EventLoop::DoPendingFunctors() {
    std::vector<Functor> functors;
    callingPendingFunctors = true;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pending_functors_);
    }

    for(const Functor& f : functors) {
        f();
    }
    callingPendingFunctors = false;
}