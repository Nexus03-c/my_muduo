#ifndef MY_MODUO_POLLER_H
#define MY_MODUO_POLLER_H
#include "source/base/nonecopyable.h"
#include "source/base/TimeStamp.h"
#include <unordered_map>
#include <vector>

class EventLoop;
class Channel;

class Poller : public nonecopyable {
public:
    using ChannelList = std::vector<Channel*>;
    Poller(EventLoop *loop);
    virtual ~Poller() = default;

    virtual TimeStamp poll(int timeout_ms, ChannelList* active_channels) = 0;
    virtual void UpdateChannel(Channel *channel) = 0;
    virtual void RemoveChannel(Channel *channel) = 0;
    virtual bool HasChannel(Channel *channel);

    static Poller* newDefaultPoller(EventLoop *loop);
protected:
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;
private:
    EventLoop* loop_;
};
#endif