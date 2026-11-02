#pragma once

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <memory>
#include <functional>
#include "Common.hpp"
#include "Connection.hpp"
#include "Log.hpp"
#include "InetAddr.hpp"

using namespace LogModule;

#define SIZE 1024

class Connection;

class Channel : public Connection
{
public:
    int GetSockFd()
    {
        return _sockfd;
    }

private:
    int _sockfd;
    std::string _inbuffer;
    std::string _outbuffer;
    InetAddr _client_addr;


};