#include "Channel.h"
#include "source/base/Logging.h"

#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd):
    loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1),
    tied_(false)
{

}

Channel::~Channel() {

}

void Channel::handleEvent(TimeStamp receive_timestamp) {
    //add code
    handleEventWithGuard(receive_timestamp);
}

void Channel::tie(const std::shared_ptr<void>&) {
    //add code
}

void Channel::update() {
    //add code
    //loop_->updateChannel(this);
}

void Channel::remove() {
    //add code
    //loop_->removeChannel(this);
}

void Channel::handleEventWithGuard(TimeStamp receive_timestamp) {
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        LOG_WARNING<<"fd = "<<fd_<<" handleEvent: EPOLLHUP";
        if(close_callback_) {
            close_callback_();
        }
    }

    if(revents_ & EPOLLERR) {
        LOG_WARNING<<"fd = "<<fd_<<" handleEvent: EPOLLERR";
        if(error_callback_) error_callback_();
    }

    if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if(read_callback_) read_callback_(receive_timestamp);
    }

    if(revents_ & EPOLLOUT) {
        if(write_callback_) write_callback_();
    }
}