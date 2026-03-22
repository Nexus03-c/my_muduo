#ifndef MY_MUDUO_TCPSERVER_H
#define MY_MUDUO_TCPSERVER_H
#include "source/base/nonecopyable.h"
#include "Callbacks.h"
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class EventLoop;
class Acceptor;
class EventLoopThreadPool;
class InetAddress;
struct sockaddr_in;

class TcpServer : public nonecopyable {
public:
    using ThreadInitFunc = std::function<void (EventLoop*)>;
    enum Option{
        kNoReusePort,
        kReusePort,
    };
    TcpServer(EventLoop* loop, const InetAddress& addr, const std::string &name, Option option = kNoReusePort);
    ~TcpServer();

    void start();
    void setThreadNums(int thread_nums);
    
    void setThreadInitCb(const ThreadInitFunc &cb) { thread_init_cb_ = std::move(cb); }
    void setConnectionCb(const ConnectionCallback &cb) { connection_cb_ = std::move(cb); }
    void setMessageCb(const MessageCallback &cb) { message_cb_ = std::move(cb); }
    void setWriteCompleteCb(const WriteCompleteCallback &cb) { write_complete_cb_ = std::move(cb); }
private:
    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;
    struct sockaddr_in getLocalAddr(int sockfd);
    void NewConnection(int conn_fd, const InetAddress &addr);
    void RemoveConnection(const TcpConnectionPtr& conn);
    void RemoveConnectionInLoop(const TcpConnectionPtr& conn);

    std::atomic<int> started_;
    std::string name_;
    std::string ip_port_;
    EventLoop* loop_;
    std::unique_ptr<Acceptor> acceptor_;
    std::unique_ptr<EventLoopThreadPool> thread_pool_;
    
    int nextconn_id_;
    ConnectionMap connetions_;

    ThreadInitFunc thread_init_cb_;
    ConnectionCallback connection_cb_;
    MessageCallback message_cb_;
    WriteCompleteCallback write_complete_cb_;
};

#endif