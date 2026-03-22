#include "TcpServer.h"
#include "EventLoop.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include "InetAddress.h"
#include "source/base/Logging.h"
#include "TcpConnection.h"

#include <sys/types.h>
#include <sys/socket.h>

TcpServer::TcpServer(EventLoop* loop, const InetAddress& addr, const std::string &name, Option option) 
  : loop_(loop),
    name_(name),
    ip_port_(addr.toIpPort()),
    acceptor_(new Acceptor(loop, addr, option == kReusePort)),
    thread_pool_(new EventLoopThreadPool(loop, name)),
    started_(0)
{
    acceptor_->setNewConnectionCb(std::bind(&TcpServer::NewConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer() {
    LOG_INFO << "TcpServer::~TcpServer [" << name_ << "] destructing";
    for(auto &item : connetions_) {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        EventLoop* io_loop = conn->loop();
        io_loop->RunInLoop(std::bind(&TcpConnection::ConnectionDestroy, conn));
    }
}

void TcpServer::start() {
    if(started_++ == 0) {
        thread_pool_->start(thread_init_cb_);
        loop_->RunInLoop(std::bind(&Acceptor::Listen, acceptor_.get()));
    }
}

void TcpServer::setThreadNums(int thread_nums) {
    thread_pool_->set_thread_nums(thread_nums);
}

void TcpServer::NewConnection(int conn_fd, const InetAddress &addr) {
    EventLoop* io_loop = thread_pool_->GetNextLoop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s%d", ip_port_.c_str(), nextconn_id_);
    nextconn_id_++;
    std::string conn_name = name_ + buf;

    LOG_INFO<<"TcpServer::NewConnection ["<<name_<<"] - new connection["<<conn_name<<"] from "<<addr.toIpPort();
    InetAddress local_addr(getLocalAddr(conn_fd));
    TcpConnectionPtr conn(new TcpConnection(io_loop, conn_name, conn_fd, local_addr, addr));
    connetions_[conn_name] = conn;
    conn->setConnectionCallback(connection_cb_);
    conn->setMessageCallback(message_cb_);
    conn->setWriteCompleteCallback(write_complete_cb_);
    conn->setCloseCallback(std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
    io_loop->RunInLoop(std::bind(&TcpConnection::ConnectionEstablished, conn));
}

void TcpServer::RemoveConnection(const TcpConnectionPtr& conn) {
    loop_->RunInLoop(std::bind(&TcpServer::RemoveConnectionInLoop, this, conn));
}

void TcpServer::RemoveConnectionInLoop(const TcpConnectionPtr& conn) {
    LOG_INFO<<"TcpServer::RemoveConnectionInLoop ["<<name_<<"] - connection "<<conn->name();
    connetions_.erase(conn->name());
    EventLoop* io_loop = conn->loop();
    io_loop->RunInLoop(std::bind(&TcpConnection::ConnectionDestroy, conn));
}

struct sockaddr_in TcpServer::getLocalAddr(int sockfd) {
    sockaddr_in local_addr;
    socklen_t len = static_cast<socklen_t>(sizeof(local_addr));
    bzero(&local_addr, 0);
    if(::getsockname(sockfd, (sockaddr*)(&local_addr), &len) < 0) {
        LOG_ERROR<<"socket getsockname";
    }
    return local_addr;
}