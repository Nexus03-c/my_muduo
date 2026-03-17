set(net_SRCS
    TcpServer.cc
    InetAddress.cc
    EventLoop.cc
    Channel.cc
    Poller.cc
    EpollPoller.cc
    DefaultPoller.cc
    EventLoopThread.cc
    Socket.cc
    Acceptor.cc
)

add_library(muduo_net SHARED ${net_SRCS})
target_link_libraries(muduo_net muduo_base)