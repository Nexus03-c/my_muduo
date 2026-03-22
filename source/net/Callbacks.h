#ifndef MY_MUDUO_CALLBACKS_H
#define MY_MUDUO_CALLBACKS_H
#include "source/base/TimeStamp.h"
#include <memory>
#include <functional>

class TcpConnection;
class Buffer;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

using ConnectionCallback = std::function<void (const TcpConnectionPtr&)>;
using CloseCallback = std::function<void (const TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void (const TcpConnectionPtr&)>;

using MessageCallback = std::function<void (const TcpConnectionPtr&, Buffer*, TimeStamp)>;
using HighWaterCallback = std::function<void (const TcpConnectionPtr&, size_t)>;
#endif