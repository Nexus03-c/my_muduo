#ifndef MY_MUDUO_TCPCONNECTION_H
#define MY_MUDUO_TCPCONNECTION_H
#include "source/base/nonecopyable.h"
#include "InetAddress.h"
#include "Buffer.h"
#include "Callbacks.h"
#include "source/base/TimeStamp.h"
#include <memory>
#include <string>
#include <any>

class EventLoop;
class Channel;
class Socket;

class TcpConnection : public nonecopyable, public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(EventLoop* loop, const std::string name, int sock_fd, const InetAddress& local_addr, const InetAddress& peer_addr);
    ~TcpConnection();

    EventLoop* loop() const { return loop_; }
    const std::string& name() const { return name_; }
    const InetAddress& localAddress() const { return local_addr_; }
    const InetAddress& peerAddress() const { return peer_addr_; }
    bool connected() const { return state_ == kConnected; }
    bool disconnected() const { return state_ == kDisconnected; }

    void send(const std::string &message);
    void ConnectionEstablished();
    void ConnectionDestroy();

    void shutdown();

    void setConnectionCallback(const ConnectionCallback& cb) { connection_cb_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { message_cb_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { write_complete_cb_ = cb; }
    void setHighWaterMarkCallback(const HighWaterCallback& cb, size_t highWaterMark) { high_water_cb_ = cb; high_water_mask_ = highWaterMark; }

    void setCloseCallback(const CloseCallback& cb) { close_cb_ = cb; }

    void setContext(const std::any& context) { context_ = std::move(context); }
    const std::any& getContext() const{ return context_; }
    std::any* getMutableContext() { return &context_; }
private:
    enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
    void sendInLoop(const std::string &message);
    void sendInLoop(const void* message, size_t len);
    void shutdownInLoop();

    void HandleRead(TimeStamp receive_time);
    void HandleWrite();
    void HandleClose();
    void HandleError();

    const char* stateToString() const;
    void setState(StateE state) { state_ = state; }

    EventLoop *loop_;
    const std::string name_;
    StateE state_;
    bool reading;
    size_t high_water_mask_;

    std::unique_ptr<Socket> socket_;
    std::unique_ptr<Channel> channel_;

    const InetAddress local_addr_;
    const InetAddress peer_addr_;

    //channel call back
    ConnectionCallback connection_cb_;
    CloseCallback close_cb_;
    MessageCallback message_cb_;
    WriteCompleteCallback write_complete_cb_;
    HighWaterCallback high_water_cb_;

    //input && output buffer
    Buffer input_buffer_;
    Buffer output_buffer_;

    std::any context_;
};

#endif