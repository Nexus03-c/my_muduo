#include "HttpContext.h"
#include "source/base/Logging.h"

HttpContext::HttpParseResult HttpContext::ParseRequest(Buffer* buffer, TimeStamp receive_time) {
    if(state_ == kRequestLine) {
        LOG_INFO<<"Begin Parse Request Line";
        const char* crlf = buffer->findCRLF();
        if(crlf == nullptr) {
            return kParseAgain;
        }
        if(!ParseRequestLine(buffer->peek(), crlf)) {
            return kParseError;
        }
        state_ = kHeaders;
        buffer->retriveUntil(crlf+2);
    }

    if(state_ == kHeaders) {
        LOG_INFO<<"Begin Parse Headers";
        HttpParseResult res = ParseHeaders(buffer, buffer->peek(), buffer->beginWrite());
        if(res == kParseAgain){
            return kParseAgain;
        }
        state_ = kDataBody;
    }

    if(state_ == kDataBody) {
        state_ = kFinish;
    }
}

bool HttpContext::ParseRequestLine(const char* begin, const char* end) {
    bool success = false;
    const char* space = std::find(begin, end, ' ');
    if(space != end && request_.set_method(begin, space)) {
        const char* start = space+1;
        space = std::find(start, end, ' ');
        if(space != end) {
            const char* question = std::find(start, space, '?');
            if(question) {
                request_.set_path(start, question);
            } else {
                request_.set_path(start, space);
            }
            start = space+1;
            const char* pos = std::find(start, end, '/');

            if(pos != end) {
                success = true;
                std::string v = std::string(pos+1, end);
                if(v == "1.0") {
                    request_.set_version(HttpRequest::kHttp10);
                }else if(v == "1.1") {
                    request_.set_version(HttpRequest::kHttp11);
                } else {
                    success = false;
                }
            }
        }
    }
    return success;
}

HttpContext::HttpParseResult HttpContext::ParseHeaders(Buffer* buffer, const char* begin, const char* end) {
    size_t size = end - begin;
    int key_start, key_end, value_start, value_end;
    int now_read_line_begin = 0;
    bool finish = false;

    for(int i = 0; i<size && !finish; i++) {
        switch(header_state_) {
            case kStart:
                key_start = i;
                now_read_line_begin = i;
                header_state_ = kKey;
                break;
            case kKey:
                if(*(begin+i) == ':') {
                    key_end = i;
                    header_state_ = kColon;
                }
                break;
            case kColon:
                if(*(begin+i) == ' ') {
                    header_state_ = kSpaceAfterColon;
                }
                break;
            case kSpaceAfterColon:
                value_start = i;
                header_state_ = kValue;
                break;
            case kValue:
                if(*(begin+i) == '\r') {
                    header_state_ = kCR;
                    value_end = i;
                }
                break;
            case kCR:
                if(*(begin+i) == '\n') {
                    header_state_ = kLF;
                    request_.AddHeader(std::string(begin+key_start, begin+key_end), std::string(begin+value_start, begin+value_end));
                }
                break;
            case kLF:
                now_read_line_begin = i;
                if(*(begin+i) == '\r') {
                    header_state_ = kEndCR;
                } else {
                    key_start = i;
                    header_state_ = kKey;
                }
                break;
            case kEndCR:
                if(*(begin+i) == '\n') {
                    header_state_ = kEndLF;
                }
                break;
            case kEndLF:
                now_read_line_begin = i;
                finish = true;
                break;
        }
    }
    buffer->retriveUntil(begin+now_read_line_begin);
    if(header_state_ == kEndLF) {
        return kParseSuccess;
    }
    header_state_ = kStart;
    return kParseAgain;
}