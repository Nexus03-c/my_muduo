#ifndef MY_MUDUO_HTTPSERVER_H
#define MY_MUDUO_HTTPSERVER_H

#include "source/net/TcpServer.h"

class HttpRequest;
class HttpResponse;

class HttpServer{
public:
    using HttpCallback = std::function<void(const HttpRequest& request, HttpResponse& response)>;
    HttpServer(EventLoop* loop,
        const InetAddress& addr, 
        const std::string &name, 
        TcpServer::Option option = TcpServer::kNoReusePort);
    void Start() { server_.start();}
    
    void setHttpCallback(const HttpCallback& cb) { http_cb_ = std::move(cb); }
    void setThreadNum(int num) { server_.setThreadNums(num); }
private:
    TcpServer server_;
    HttpCallback http_cb_;

    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr&, Buffer*, TimeStamp);
    void onRequest(const TcpConnectionPtr&, const HttpRequest&);

    void handleError(const TcpConnectionPtr&);
};

#endif