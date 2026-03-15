#ifndef MY_MUDUO_SOCKET_H
#define MY_MUDUO_SOCKET_H
#include "source/base/nonecopyable.h"

class InetAddress;

class Socket : public nonecopyable {
public:
    explicit Socket(int sock_fd)
      : sock_fd_(sock_fd)
    {

    }

    ~Socket();

    int fd() const { return sock_fd_; }
    void BindAddress(const InetAddress &local_address);
    void Listen();
    int Accept(InetAddress *peer_addr);
    void ShutDownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);
private:
    const int sock_fd_;
};
#endif