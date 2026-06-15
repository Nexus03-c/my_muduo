#include "TcpConnection.h"
#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"
#include "source/base/Logging.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <functional>
#include <unistd.h>

TcpConnection::TcpConnection(EventLoop* loop, const std::string name, int sock_fd, const InetAddress& local_addr, const InetAddress& peer_addr) 
  : loop_(loop),
    name_(name),
    reading(true),
    state_(kConnecting),
    local_addr_(local_addr),
    peer_addr_(peer_addr),
    socket_(new Socket(sock_fd)),
    channel_(new Channel(loop, sock_fd)),
    high_water_mask_(64 * 1024 * 1024)
{
    channel_->setReadCallback(std::bind(&TcpConnection::HandleRead, this, std::placeholders::_1));
    channel_->setWriteCallback(std::bind(&TcpConnection::HandleWrite, this));
    channel_->setCloseCallback(std::bind(&TcpConnection::HandleClose, this));
    channel_->setErrorCallback(std::bind(&TcpConnection::HandleError, this));
    LOG_INFO<<"TcpConnection::ctor["<<name_<<"] at" <<" fd ="<<sock_fd;
    socket_->setKeepAlive(true);
}

TcpConnection::~TcpConnection() {
    LOG_INFO<<"TcpConnection::dtor["<<name_<<"] at" <<" fd ="<<channel_->fd()<<" state="<<stateToString();
}

void TcpConnection::send(const std::string &message) {
    if(state_ == kConnected) {
        if(loop_->IsInLoopThread()) {
            sendInLoop(message.c_str(), message.size());
        } else {
            void (TcpConnection::*fp)(const std::string&) = &TcpConnection::sendInLoop;
            loop_->RunInLoop(std::bind(fp, this, std::string(message.c_str())));
        }
    }
}

void TcpConnection::sendInLoop(const std::string &message) {
    sendInLoop(message.c_str(), message.size());
}

void TcpConnection::sendInLoop(const void* data, size_t len) {
    ssize_t nwrote = 0;
    ssize_t remaining = len;
    bool fault_error = false;
    if(state_ == kDisconnected) {
        LOG_WARNING<<"TcpConnection disconnected, give up writing";
        return;
    }

    if(!channel_->isWriting() && output_buffer_.readableBytes() == 0) {
        nwrote = ::write(channel_->fd(), data, len);
        if(nwrote >= 0) {
            remaining -= nwrote;
            if(remaining == 0 && write_complete_cb_) {
                loop_->QueueInLoop(std::bind(write_complete_cb_, shared_from_this()));
            }
        } else {
            nwrote = 0;
            if(errno != EWOULDBLOCK) {
                LOG_ERROR<<"TcpConnection::sendInLoop";
                if(errno == EPIPE || errno == ECONNRESET) {
                    fault_error = true;
                }
            }
        }
    }

    if(!fault_error && remaining>0) {
        size_t old_len = output_buffer_.readableBytes();
        if(old_len+remaining > high_water_mask_ && old_len < high_water_mask_ && high_water_cb_) {
            loop_->QueueInLoop(std::bind(high_water_cb_, shared_from_this(), old_len+remaining));
        }
        output_buffer_.append(data+nwrote, remaining);
        if(!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

void TcpConnection::ConnectionEstablished() {
    setState(kConnected);
    channel_->tie(shared_from_this());
    channel_->enableReading();

    connection_cb_(shared_from_this());
}

void TcpConnection::ConnectionDestroy() {
    if(state_ == kConnected) {
        setState(kDisconnected);
        channel_->disableAll();

        connection_cb_(shared_from_this());
    }
    channel_->remove();
}

void TcpConnection::shutdown() {
    if(state_ == kConnected) {
        setState(kDisconnecting);
        loop_->RunInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
    }
}

void TcpConnection::shutdownInLoop() {
    if(!channel_->isWriting()) {
        socket_->ShutDownWrite();
    }
}

void TcpConnection::HandleRead(TimeStamp receive_time) {
    int save_errno = 0;
    ssize_t n = input_buffer_.readFd(channel_->fd(), &save_errno);
    if(n > 0) {
        LOG_INFO<<"handle message";
        message_cb_(shared_from_this(), &input_buffer_, receive_time);
    } else if( n == 0) {
        HandleClose();
    } else {
        LOG_ERROR<<"TcpConnection::HandleRead error";
        HandleError();
    }
}

void TcpConnection::HandleWrite() {
    if(channel_->isWriting()) {
        ssize_t nwrote = ::write(channel_->fd(), output_buffer_.peek(), output_buffer_.readableBytes());
        if(nwrote > 0) {
            output_buffer_.retrieve(nwrote);
            if(output_buffer_.readableBytes() == 0) {
                channel_->disableWriting();
                if(write_complete_cb_) {
                    loop_->QueueInLoop(std::bind(write_complete_cb_, shared_from_this()));
                }
                if(state_ == kDisconnecting) {
                    shutdownInLoop();
                }
            }
        } else {
            LOG_ERROR<<"TcpConnection::HandleWrite";
        }
    } else {
        LOG_INFO<<"Connection fd = "<<channel_->fd()<<" is down, no more writing";
    }
}

void TcpConnection::HandleClose() {
    LOG_INFO<<"fd = "<<channel_->fd()<<" state = "<<stateToString();
    setState(kDisconnected);
    channel_->disableAll();

    TcpConnectionPtr guard_this(shared_from_this());
    connection_cb_(guard_this);
    close_cb_(guard_this);
}

void TcpConnection::HandleError() {
    int err;
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof(optval));
    int res = ::getsockopt(channel_->fd(), SOL_SOCKET, SO_ERROR, &optval, &optlen);
    if(res < 0) {
        err = errno;
    } else {
        err = optval;
    }
    LOG_ERROR<<"TcpConnection::HandleError ["<<name_<<"] - SO_ERROR = "<<err<<" "<<strerror(err);
}

const char* TcpConnection::stateToString() const
{
  switch (state_)
  {
    case kDisconnected:
      return "kDisconnected";
    case kConnecting:
      return "kConnecting";
    case kConnected:
      return "kConnected";
    case kDisconnecting:
      return "kDisconnecting";
    default:
      return "unknown state";
  }
}