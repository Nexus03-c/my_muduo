#ifndef MY_MUDUO_INETADDRESS_H
#define MY_MUDUO_INETADDRESS_H
#include <netinet/in.h>
#include <string>

class InetAddress {
public:
    explicit InetAddress(const sockaddr_in& addr): addr_(addr) {}
    InetAddress(std::string ip_str, uint16_t port);

    sa_family_t sa_family() const {return addr_.sin_family;}
    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t port() const;
    const sockaddr_in* getSockAddr() const { return &addr_; }
    void setSockAddr(const sockaddr_in &addr) { addr_ = addr; }
private:
    sockaddr_in addr_;
};
#endif