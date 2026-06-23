#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

class InetAddr
{
public:
    InetAddr(struct sockaddr_in &addr) : _addr(addr)
    {
        _port = ntohs(_addr.sin_port);   // 从网络中拿到的！网络序列
        _ip = inet_ntoa(_addr.sin_addr); // 4字节网络风格的IP -> 点分十进制的字符串风格的IP
    }

    uint16_t Port() { return _port; }
    std::string Ip() { return _ip; }
    const struct sockaddr_in &NetAddr() { return _addr; }
    bool operator==(const InetAddr &addr)
    {
        return addr._ip == _ip && addr._port == _port;
    }
    std::string StringAddr()
    {
        return _ip + ":" + std::to_string(_port);
    }
    ~InetAddr()
    {
    }

private:
    struct sockaddr_in _addr;
    std::string _ip;
    uint16_t _port;
};