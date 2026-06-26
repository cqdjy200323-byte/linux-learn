#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include "Common.hpp"
#include "Log.hpp"
#include "InetAddr.hpp"

namespace SocketModule
{

    using namespace LogModule;

    const static int gbacklog = 16;
    class Socket
    {
    public:
        virtual ~Socket(){}
        virtual void SocketOrDie() = 0;
        virtual void BindOrDie(uint16_t port) = 0;
        virtual void ListenOrDie(int backlog) = 0;
        virtual std::shared_ptr<Socket> Accept(InetAddr* client)= 0;
        virtual void Close() = 0;

    public:
        void BuildTcpSocketMethod(uint16_t port,int backlog=gbacklog)
        {
            SocketOrDie();
            BindOrDie(port);
            ListenOrDie(backlog);
        }

    private:

    };


    class TcpServer : public Socket
    {
    public:

        TcpServer(int socket)
        :_socket(socket)
        {
        }


        void SocketOrDie() override
        {
            _socket = ::socket(AF_INET, SOCK_STREAM, 0);
            if(_socket<0)
            {
                LOG(LogLevel::FATAL) << "socket error";
                exit(SOCKET_ERR);
            }
            LOG(LogLevel::INFO) << "socket success";

        }

        void BindOrDie(uint16_t port) override
        {
            InetAddr localaddr(port);
            int n = ::bind(_socket, localaddr.NetAddrPtr(), localaddr.NetAddrLen());
            if(n<0)
            {
                LOG(LogLevel::FATAL) << "bind error";
                exit(BIND_ERR);
            }
            LOG(LogLevel::INFO) << "bind success";


        }

        void ListenOrDie(int backlog) override
        {
            int n = ::listen(_socket, backlog);
            if (n < 0)
            {
                LOG(LogLevel::FATAL) << "listen error";
                exit(LISTEN_ERR);
            }
            LOG(LogLevel::INFO) << "listen success";
        }

        std::shared_ptr<Socket> Accept(InetAddr* client) override
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int fd = ::accept(_socket, CONV(peer), &len);
             if(fd < 0)
            {
                LOG(LogLevel::WARNING) << "accept warning ...";
                return nullptr; 
            }
            client->SetInetAddr(peer);
            return std::make_shared<TcpServer>(fd);
        }

        void Close()
        {
            if(_socket >= 0)
                ::close(_socket);
        }

    private:
        int _socket;
    };
}
