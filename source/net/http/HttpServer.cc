#include "HttpServer.h"
#include "HttpContext.h"
#include "HttpResponse.h"
#include "source/net/TcpConnection.h"
#include "source/base/Logging.h"

HttpServer::HttpServer(EventLoop* loop,
        const InetAddress& addr, 
        const std::string &name, 
        TcpServer::Option option): server_(loop, addr, name, option)
{
    server_.setConnectionCb(std::bind(&HttpServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCb(std::bind(&HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void HttpServer::onConnection(const TcpConnectionPtr& conn) {
    if(conn->connected()) {
        conn->setContext(HttpContext());
        LOG_INFO<<"Connection Up: "<<conn->peerAddress().toIpPort();
    } else if(conn->disconnected()) {
        LOG_INFO<<"Connection Down: "<<conn->peerAddress().toIpPort();
    }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, TimeStamp receive_time) {
    HttpContext* http_context = std::any_cast<HttpContext>(conn->getMutableContext());

    HttpContext::HttpParseResult parse_result = http_context->ParseRequest(buffer, receive_time);
    if(parse_result == HttpContext::kParseAgain) {
        return;
    } else if(parse_result == HttpContext::kParseError) {
        handleError(conn);
        return;
    }

    if(http_context->Finish()) {
        onRequest(conn, http_context->request());
        http_context->Reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& request) {
    HttpResponse response;
    http_cb_(request, response);
    Buffer buf;
    response.AppendToBuffer(&buf);
    conn->send(buf.retrieveAllAsString());
    conn->shutdown();
}

void HttpServer::handleError(const TcpConnectionPtr& conn) {
    conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
    conn->shutdown();
}