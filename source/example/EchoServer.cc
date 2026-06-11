#include "source/net/TcpServer.h"
#include "source/net/InetAddress.h"
#include "source/net/EventLoop.h"
#include "source/net/TcpConnection.h"
#include "source/base/Logging.h"
#include "source/base/AsyncLogging.h"

#include <string>
#include <functional>

AsyncLogging* log = nullptr;

void OutputFunc(const char *str, int len) {
    log->append(str, len);
}

class EchoServer {
public:
    EchoServer(EventLoop* loop, const InetAddress& addr, const std::string &name)
      : server_(loop, addr, name)
    {
        server_.setConnectionCb(std::bind(&EchoServer::HandleConnection, this, std::placeholders::_1));
        server_.setMessageCb(std::bind(&EchoServer::HanldeMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        server_.setThreadNums(4);
    }
    
    void start() { server_.start(); }
private:
    void HandleConnection(const TcpConnectionPtr& conn) {
        if(conn->connected()) {
            LOG_INFO<<"Connection Up: "<<conn->peerAddress().toIpPort();
        } else {
            LOG_INFO<<"Connection Down: "<<conn->peerAddress().toIpPort();
        }
    }
    void HanldeMessage(const TcpConnectionPtr& conn, Buffer* buf, TimeStamp receive_time) {
        std::string str = buf->retrieveAllAsString();
        LOG_INFO<<"Receive message from connection "<<conn->name()<<" "<<"msg: "<<str;
        conn->send(str);
        conn->shutdown();
    }
    TcpServer server_;
};

int main() {
    Logger::setLogLevel(Logger::DEBUG);
    log = new AsyncLogging();
    Logger::setOutput(OutputFunc);
    EventLoop loop;
    InetAddress addr("127.0.0.1", 8000);
    EchoServer server(&loop, addr, "EchoServer-01");
    server.start();
    loop.Loop();

    return 0;
}