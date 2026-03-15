#include "Socket.h"
#include "source/base/Logging.h"
#include "InetAddress.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

Socket::~Socket() {
    close(sock_fd_);
}

void Socket::BindAddress(const InetAddress &local_address) {
    if(::bind(sock_fd_, (sockaddr*)(local_address.getSockAddr()), sizeof(sockaddr)) < 0) {
        LOG_FATAL<<"socket bind failed";
    }
}

void Socket::Listen() {
    if(::listen(sock_fd_, SOMAXCONN) < 0) {
        LOG_FATAL<<"socket listen failed";
    }
}

int Socket::Accept(InetAddress *peer_addr) {
    socklen_t sock_len;
    sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof(sockaddr_in));
    int conn_fd = ::accept4(sock_fd_, (sockaddr*)&sock_addr, &sock_len, SOCK_CLOEXEC | SOCK_NONBLOCK);
    if(conn_fd >= 0) {
        peer_addr->setSockAddr(sock_addr);
    }
    return conn_fd;
}

void Socket::ShutDownWrite() {
    if(::shutdown(sock_fd_, SHUT_WR) < 0) {
        LOG_ERROR<<"socket shutdown write failed";
    }
}

void Socket::setTcpNoDelay(bool on) {
    int opt_val = on? 1 : 0;
    ::setsockopt(sock_fd_, IPPROTO_TCP, TCP_NODELAY, &opt_val, sizeof(opt_val));
}

void Socket::setReuseAddr(bool on) {
    int opt_val = on? 1 : 0;
    ::setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
}

void Socket::setReusePort(bool on) {
    int opt_val = on? 1 : 0;
    ::setsockopt(sock_fd_, SOL_SOCKET, SO_REUSEPORT, &opt_val, sizeof(opt_val));
}

void Socket::setKeepAlive(bool on) {
    int opt_val = on? 1 : 0;
    ::setsockopt(sock_fd_, SOL_SOCKET, SO_KEEPALIVE, &opt_val, sizeof(opt_val));
}