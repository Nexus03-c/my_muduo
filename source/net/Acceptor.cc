#include "Acceptor.h"
#include "InetAddress.h"
#include "source/base/Logging.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

static int CreateNoneBlockingSocket() {
    int socket_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_TCP);
    if(socket_fd < 0) {
        LOG_FATAL<<"create socket fd failed";
    }
    return socket_fd;
}

Acceptor::Acceptor(EventLoop* loop, const InetAddress &addr, bool reuseport) 
  : listening_(false),
    loop_(loop),
    accept_socket_(CreateNoneBlockingSocket()),
    accept_channel_(loop, accept_socket_.fd())
{
    accept_socket_.setReuseAddr(true);
    accept_socket_.setReusePort(true);
    accept_socket_.BindAddress(addr);
    accept_channel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    accept_channel_.disableAll();
    accept_channel_.remove();
}

void Acceptor::Listen() {
    listening_ = true;
    accept_socket_.Listen();
    accept_channel_.enableReading();
}

void Acceptor::handleRead() {
    InetAddress peer_addr;
    int conn_fd = accept_socket_.Accept(&peer_addr);
    if(conn_fd >= 0) {
        if(newconnection_cb_) {
            newconnection_cb_(conn_fd, peer_addr);
        } else {
            ::close(conn_fd);
        }
    } else {
        if(errno == EMFILE) {
            LOG_ERROR<<"Reach fd max limit";
        }
    }
}