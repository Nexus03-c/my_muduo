#include "HttpResponse.h"

void HttpResponse::AppendToBuffer(Buffer* buffer) {
    char buf[32];
    snprintf(buf, sizeof buf, "%s %d ", version_.c_str(), status_);
    buffer->append(buf);
    buffer->append(status_msg_);
    buffer->append("\r\n");

    snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
    buffer->append(buf);
    buffer->append("Connection: close\r\n");

    for(const auto& header : headers) {
        buffer->append(header.first);
        buffer->append(": ");
        buffer->append(header.second);
        buffer->append("\r\n");
    }

    buffer->append("\r\n");
    buffer->append(body_);
}