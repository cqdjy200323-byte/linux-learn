#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Log.hpp"
#include "InetAddr.hpp"
#include <cstring>
#include <unistd.h>
#include "common.hpp"
#include "ThreadPool.hpp"

using namespace LogModule;
using namespace ThreadPoolModule;

using func_t = std::function<void(const std::string&,InetAddr &)>;
const static int backlog = 8;
class TcpServer
{
public:
    TcpServer(uint16_t port,func_t func)
        : _port(port)
        ,_func(func)
    {
    }

    ~TcpServer()
    {
    }

    class ThreadData
    {
    public:
        ThreadData(int sockfd, InetAddr &addr, TcpServer *td)
            : _sockfd(sockfd), _addr(addr), _tsvr(td)
        {
        }

    public:
        int _sockfd;
        InetAddr _addr;
        TcpServer *_tsvr;
    };

    void Init()
    {
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensockfd < 0)
        {
            LOG(LogLevel::FATAL) << "socker error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "socker success" << _listensockfd;

        InetAddr local(_port);
        int n = bind(_listensockfd, local.NetAddrPtr(), local.NetAddrLen());
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO) << "bind success: " << _listensockfd; // 3

        n = listen(_listensockfd, backlog);
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "listen error";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::INFO) << "listen success: " << _listensockfd; // 3
    }

    void Service(int sockfd, InetAddr &addr)
    {
        char buffer[1024];
        while (true)
        {
            ssize_t n = read(sockfd, buffer, sizeof(buffer) - 1);
            if (n > 0)
            {
                buffer[n] = 0;
                LOG(LogLevel::DEBUG) << addr.StringAddr() << " #" << buffer;
                _func(buffer, addr);
            }
            else if (n == 0)
            {
                LOG(LogLevel::DEBUG) << addr.StringAddr() << " 退出了...";
                close(sockfd);
                break;
            }
            else
            {
                LOG(LogLevel::DEBUG) << addr.StringAddr() << " 异常...";
                close(sockfd);
                break;
            }
        }
    }
    static void *Routine(void *args)
    {
        pthread_detach(pthread_self());
        ThreadData *td = static_cast<ThreadData *>(args);
        td->_tsvr->Service(td->_sockfd, td->_addr);
        delete td;
        return nullptr;
    }

    void Run()
    {
        _isrunning = true;
        while (_isrunning)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            int sockfd = accept(_listensockfd, CONV(peer), &len);
            if (sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error";
                continue;
            }
            InetAddr addr(peer);
            // 线程池
            // InetAddr addr(peer);
            // ThreadPool<task_t>::GetInstance()->Enqueue(
            //     [this, sockfd, &addr]()
            //     { this->Service(sockfd, addr); });

            // 多线程
            ThreadData *td = new ThreadData(sockfd, addr, this);
            pthread_t tid;
            pthread_create(&tid, nullptr, Routine, td);
        }
        _isrunning = false;
    }

private:
    uint16_t _port;
    int _listensockfd;
    bool _isrunning;
    func_t _func;
};