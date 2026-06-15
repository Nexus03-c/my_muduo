#ifndef MY_MUDUO_HTTPCONTEXT_H
#define MY_MUDUO_HTTPCONTEXT_H

#include "HttpRequest.h"
#include "source/net/Buffer.h"

class HttpContext {
public:
    enum HttpParseState {
        kRequestLine,
        kHeaders,
        kDataBody,
        kFinish,
    };
    enum HttpParseResult {
        kParseSuccess,
        kParseAgain,
        kParseError,
    };
    enum HttpHeaderState {
        kStart,
        kKey,
        kColon,
        kSpaceAfterColon,
        kValue,
        kCR,
        kLF,
        kEndCR,
        kEndLF,
    };

    HttpContext(): state_(kRequestLine), header_state_(kStart){}

    HttpParseResult ParseRequest(Buffer* buffer, TimeStamp receive_time);
    bool Finish() { return state_ == kFinish;}
    void Reset() {
        state_ = kRequestLine;
        header_state_ = kStart;
        HttpRequest temp_request;
        request_.swap(temp_request);
    }
    
    HttpRequest& request() { return request_; }
    const HttpRequest& request() const{ return request_; }
private:
    HttpParseState state_;
    HttpHeaderState header_state_;
    HttpRequest request_;

    bool ParseRequestLine(const char* begin, const char* end);
    HttpParseResult ParseHeaders(Buffer* buffer, const char* begin, const char* end);
};

#endif