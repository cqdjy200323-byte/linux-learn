#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "common.hpp"

class InetAddr
{
public:
    InetAddr(struct sockaddr_in &addr) : _addr(addr)
    {
        _port = ntohs(_addr.sin_port);
        char ipbuffer[64];
        inet_ntop(AF_INET, &_addr.sin_addr, ipbuffer, sizeof(_addr));
        _ip = ipbuffer;
    }

    InetAddr(const std::string &ip,uint16_t port)
    :_ip(ip)
    ,_port(port)
    {
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family=AF_INET;
        _addr.sin_port=htons(_port);
        inet_pton(AF_INET, _ip.c_str(),&_addr.sin_addr);

    }
    InetAddr(uint16_t port)
    :_port(port)
    {
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_addr.s_addr = INADDR_ANY;
        _addr.sin_port = htons(_port);

    }

    uint16_t Port() { return _port; }
    std::string Ip() { return _ip; }
    const struct sockaddr_in &NetAddr() { return _addr; }
    const struct sockaddr* NetAddrPtr(){
        return CONV(_addr);
    }

    socklen_t NetAddrLen()
    {
        return sizeof(_addr);
    }

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