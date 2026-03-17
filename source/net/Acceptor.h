#ifndef MY_MUDUO_ACCEPTOR_H
#define MY_MUDUO_ACCEPTOR_H
#include "source/base/nonecopyable.h"
#include "Channel.h"
#include "Socket.h"

#include <functional>

class EventLoop;
class InetAddress;

class Acceptor : public nonecopyable {
public:
    using NewConnectionCallback = std::function<void(int socket, const InetAddress&)>;
    Acceptor(EventLoop* loop, const InetAddress &addr, bool reuseport);
    ~Acceptor();

    void Listen();
    bool listening() const { return listening_; }
    void setNewConnectionCb(const NewConnectionCallback& cb) { newconnection_cb_ = cb; }
private:
    void handleRead();

    bool listening_;
    EventLoop* loop_;
    Socket accept_socket_;
    Channel accept_channel_;
    NewConnectionCallback newconnection_cb_;
};

#endif