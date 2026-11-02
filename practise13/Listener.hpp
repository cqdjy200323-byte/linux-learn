#pragma once

#include <iostream>
#include <memory>
#include "Epoller.hpp"
#include "Socket.hpp"
#include "Connection.hpp"
#include "Common.hpp"

using namespace SocketModule;

class Listener :public Connection
{
public:
    Listener(int port=defaultport)
    :_port(port)
    ,_listensock(std::make_unique<TcpSocket>())
    {
        _listensock->BuildTcpSocketMethod(_port);
        SetEvent(EPOLLIN);
    }

    int GetSockFd()
    {
        return _listensock->Fd();
    }

    ~Listener()
    {

    }


    void Recver()
    {}
    void Send()
    {}
    void Excepter()
    {}

private:
    int _port;
    std::unique_ptr<Socket> _listensock;
};