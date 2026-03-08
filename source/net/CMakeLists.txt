set(net_SRCS
    TcpServer.cc
#    InetAddress.cc
    EventLoop.cc
    Channel.cc
    Poller.cc
)

add_library(muduo_net SHARED ${net_SRCS})
target_link_libraries(muduo_net muduo_base)