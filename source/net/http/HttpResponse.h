#ifndef MY_MUDUO_HTTPRESPONSE_H
#define MY_MUDUO_HTTPRESPONSE_H

#include "source/net/Buffer.h"
#include <unordered_map>

class HttpResponse {
public:
    enum HttpStatus {
        kUnKnown,
        kOk = 200,
        kPermanently = 301,
        kBadRequest = 400,
        kNotFound = 404,
    };
    HttpResponse(): status_(kUnKnown){}

    void set_status(HttpStatus status) { status_ = status; }
    void set_status_msg(const std::string& msg) { status_msg_ = msg; }
    void add_header(const std::string& key, const std::string& value) { headers[key] = value; }
    void set_body(const std::string& body) { body_ = std::move(body); }
    void set_version(const std::string& version) { version_ = std::move(version); }
    void SetContentType(const std::string& type) {
        add_header("Content-Type", type);
    }

    void AppendToBuffer(Buffer* buffer);
private:
    HttpStatus status_;
    std::string status_msg_;
    std::string body_;
    std::string version_;
    std::unordered_map<std::string, std::string> headers;
};

#endif