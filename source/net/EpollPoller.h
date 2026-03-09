#ifndef MY_MUDUO_EPOLLPOLLER_H
#define MY_MUDUO_EPOLLPOLLER_H
#include "Poller.h"
#include <vector>

struct epoll_event;
class Channel;

class EpollPoller : public Poller {
public:
    //epoll_create
    EpollPoller(EventLoop* loop);
    ~EpollPoller();

    //epoll_ctl
    void UpdateChannel(Channel *channel) override;
    void RemoveChannel(Channel *channel) override;

    //epoll_wait
    TimeStamp poll(int timeout_ms, ChannelList *active_channels) override;
private:
    using EventList = std::vector<epoll_event>;
    static const int kInitEventListSize = 16;
    static const int kNew = -1;//not add to epoll_fd & channels_
    static const int kAdded = 1;//add to epoll_fd & channels_
    static const int kDeleted = 2;//add to channels & delete at epoll_fd

    void update(int operation, Channel *channel);
    void fillActiveChannels(int num_events, ChannelList *active_channels);

    int epoll_fd_;
    EventList events_;
};

#endif