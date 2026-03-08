#ifndef MY_MUDUO_CHANNEL_H
#define MY_MUDUO_CHANNEL_H

#include "source/base/nonecopyable.h"
#include "source/base/TimeStamp.h"

#include <functional>
#include <memory>

class EventLoop;

class Channel : public nonecopyable {
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(TimeStamp)>;

    Channel(EventLoop *loop, int fd);
    ~Channel();
    //event callback
    void handleEvent(TimeStamp receive_timestamp);
    //set event callback
    void setReadCallback(ReadEventCallback cb) { read_callback_ = std::move(read_callback_); }
    void setWriteCallback(EventCallback cb) { write_callback_ = std::move(cb); }
    void setCloseCallback(EventCallback cb) { close_callback_ = std::move(cb); }
    void setErrorCallback(EventCallback cb) { error_callback_ = std::move(cb); }

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revents) { revents_ = revents;}

    void tie(const std::shared_ptr<void>&);

    //set fd_ event
    void enableReading() { events_ |= kReadEvent; }
    void disableReading() { events_ &= ~kReadEvent; }
    void enableWriting() { events_ |= kWriteEvent; }
    void disableWriting() { events_ &= ~kWriteEvent; }
    void disableAll() { events_ = kNoneEvent; }
    //get fd_ event
    bool isReading() { return events_ & kReadEvent; }
    bool isWriting() { return events_ & kWriteEvent; }
    bool isNoneEvent() { return events_ == kNoneEvent; }
    //index
    int index() { return index_; }
    int set_index(int index) { index_ = index; }

    EventLoop* ownerLoop() { return loop_;}
    void remove();
private:
    void update();
    void handleEventWithGuard(TimeStamp receive_timestamp);
    
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;
    int fd_;
    int events_;
    int revents_;
    int index_;

    std::weak_ptr<void> tie_;
    bool tied_;

    ReadEventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback close_callback_;
    EventCallback error_callback_;
};

#endif