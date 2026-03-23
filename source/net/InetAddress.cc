#include "InetAddress.h"
#include <string.h>
#include <arpa/inet.h>

InetAddress::InetAddress(std::string ip_str, uint16_t port) {
    memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    inet_pton(AF_INET, ip_str.c_str(), &addr_.sin_addr);
}

std::string InetAddress::toIp() const{
    char buf[64] = {0};
    inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf);
    return buf;
}
std::string InetAddress::toIpPort() const{
    char buf[64] = {0};
    inet_ntop(AF_INET, &addr_.sin_addr, buf, sizeof buf);
    size_t len = strlen(buf);
    snprintf(buf+len, sizeof(buf) - len, ":%u", port());
    return buf;
}
uint16_t InetAddress::port() const{
    return ntohs(addr_.sin_port);
}