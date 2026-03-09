#include "EpollPoller.h"
#include "Channel.h"
#include "source/base/Logging.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>

EpollPoller::EpollPoller(EventLoop* loop) :
    Poller(loop),
    epoll_fd_(epoll_create1(EPOLL_CLOEXEC)),
    events_(kInitEventListSize)
{

}

EpollPoller::~EpollPoller() {
    close(epoll_fd_);
}

void EpollPoller::UpdateChannel(Channel *channel) {
    const int index = channel->index();
    LOG_DEBUG<<"fd = "<<channel->fd()<<" events = "<<channel->events()<<" index = "<<channel->index();
    if(index == kNew || index == kDeleted) {
        int fd = channel->fd();
        if(index == kNew) {
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        if(channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::RemoveChannel(Channel *channel) {
    int fd = channel->fd();
    int index = channel->index();
    LOG_DEBUG<<"remove fd = "<<fd;
    channels_.erase(fd);
    
    if(index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

TimeStamp EpollPoller::poll(int timeout_ms, ChannelList *active_channels) {
    int num_events = epoll_wait(epoll_fd_, &*events_.begin(), events_.size(), timeout_ms);
    TimeStamp now(TimeStamp::now());
    int save_erro = errno;
    if(num_events > 0) {
        LOG_DEBUG<<"events happen";
        fillActiveChannels(num_events, active_channels);
        if(static_cast<size_t>(num_events) == events_.size()) {
            events_.resize(2 * num_events);
        }
    } else if(num_events == 0) {
        LOG_DEBUG<<"nothing happen";
    } else {
        if(save_erro != EINTR) {
            LOG_ERROR<<"EPollPoller::poll() errno: "<<save_erro;
        }
    }
    return now;
}

void EpollPoller::update(int operation, Channel *channel) {
    int fd = channel->fd();
    epoll_event event;
    event.events = channel->events();
    event.data.ptr = static_cast<void*>(channel);

    if(epoll_ctl(epoll_fd_, operation, fd, &event) < 0) {
        if(operation == EPOLL_CTL_DEL) {
            LOG_ERROR<<"epoll_ctl EPOLL_CTL_DEL failed";
        } else if(operation == EPOLL_CTL_MOD){
            LOG_ERROR<<"epoll_ctl EPOLL_CTL_MOD failed";
        } else {
            LOG_ERROR<<"epoll_ctl EPOLL_CTL_ADD failed";
        }
    }
}

void EpollPoller::fillActiveChannels(int num_events, ChannelList *active_channels) {
    for(int i = 0; i<num_events; i++) {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        active_channels->push_back(channel);
    }
}