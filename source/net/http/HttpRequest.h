#ifndef MY_MUDUO_HTTPREQUEST_H
#define MY_MUDUO_HTTPREQUEST_H

#include "source/base/TimeStamp.h"
#include <string>
#include <map>

class HttpRequest {
public:
    enum Method {
        kInvalid,
        kGet,
        kPost,
        kHead,
    };
    enum Version {
        kUnKnown,
        kHttp10,
        kHttp11,
    };

    HttpRequest(): method_(kInvalid), version_(kUnKnown){}

    void set_version(Version version) { version_ = version; }
    Version version() const{ return version_; }

    Method method() const{ return method_; }
    bool set_method(const char* begin, const char* end) {
        std::string m = std::string(begin, end);

        if(m == "GET") {
            method_ = kGet;
        } else if(m == "POST") {
            method_ = kPost;
        } else if(m == "HEAD") {
            method_ = kHead;
        } else {
            method_ = kInvalid;
        }
        return method_ != kInvalid;
    }
    std::string MethodToString() const{
        const char* method_str = "UnKnown";
        switch(method_) {
            case kGet:
                method_str = "GET";
                break;
            case kPost:
                method_str = "POST";
                break;
            case kHead:
                method_str = "HEAD";
                break;
        }
        return method_str;
    }

    void set_path(const char* begin, const char* end) {
        path_.assign(begin, end);
    }
    const std::string& path() const{ return path_; }

    TimeStamp receive_time() const{ return receive_time_; }
    void set_receive_time(TimeStamp receive_time) { receive_time_ = receive_time; }

    void AddHeader(const std::string& key, const std::string& value) {
        headers_[key] = value;
    };
    std::string GetHeader(std::string key) const {
        std::string result;
        if(headers_.find(key) != headers_.end()) {
            result = headers_.find(key)->second;
        }
        return result;
    };

    void swap(HttpRequest& request) {
        std::swap(method_, request.method_);
        std::swap(version_, request.version_);
        path_.swap(request.path_);
        receive_time_.swap(request.receive_time_);
        headers_.swap(request.headers_);
    }
private:
    Method method_;
    Version version_;
    std::string path_;
    TimeStamp receive_time_;
    std::map<std::string, std::string> headers_;
};

#endif