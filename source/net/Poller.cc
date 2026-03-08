#include "Poller.h"
#include "Channel.h"

Poller::Poller(EventLoop *loop):
    loop_(loop)
{

}

bool Poller::HasChannel(Channel *channel) {
    auto ite = channels_.find(channel->fd());
    return ite != channels_.end() && ite->second == channel;
}