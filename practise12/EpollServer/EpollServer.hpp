#pragma once

#include <iostream>
#include <memory>
#include <unistd.h>
#include <sys/epoll.h>
#include "Socket.hpp"
#include "Log.hpp"

using namespace LogModule;
using namespace SocketModule;
class SelectServer
{
    const static int size = 64;
    const static int defaultfd = -1;

public:
    SelectServer(int port)
        : _listensock(std::make_unique<TcpSocket>()), _isrunning(false)
    {
        _listensock->BuildTcpSocketMethod(port);
        _epfd = epoll_create(256);
        if (_epfd < 0)
        {
            LOG(LogLevel::FATAL) << "epoll_create error";
            exit(EPOLL_CREATE_ERR);
        }
        LOG(LogLevel::INFO) << "epoll_create success: " << _epfd; // 4
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = _listensock->Fd();
        int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, _listensock->Fd(), &ev);
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "add listensockfd failed";
            exit(EPOLL_CTL_ERR);
        }
    }

    void Start()
    {
        _isrunning = true;
        int timeout = -1;
        while (_isrunning)
        {
            int n = epoll_wait(_epfd, _revs, size, timeout);
            switch (n)
            {
            case -1:
                LOG(LogLevel::ERROR) << "epoll error";
                break;
            case 0:
                LOG(LogLevel::INFO) << " epoll time out...";
                break;
            default:
                // 有事件就绪,就不仅仅是新连接到来了吧？读事件就绪啊？
                LOG(LogLevel::DEBUG) << "有事件就绪了..., n : " << n;
                Dispatcher(n); // 处理就绪的事件啊！
                break;
            }
        }

        _isrunning = false;
    }

    void Dispatcher(int n)
    {

        LOG(LogLevel::DEBUG) << "event ready ..."; // LT: 水平触发模式--epoll默认
        for (int i = 0; i < n; i++)
        {
            int sockfd = _revs[i].data.fd;
            uint32_t revent = _revs[i].events;
            if (revent & EPOLLIN)
            { // 读事件就绪
                // listensockfd ready? normal socfd ready？？
                if (sockfd == _listensock->Fd())
                {
                    // 读事件就绪 && 新连接到来
                    Accepter();
                }
                else
                {
                    // 读事件就绪 && 普通socket可读
                    Recver(sockfd);
                }
            }
        }
    }

    void Accepter()
    {
        InetAddr client;
        int sockfd = _listensock->Accept(&client); // accept会不会阻塞?
        if (sockfd >= 0)
        {
            // 获取新链接到来成功, 然后呢？？能不能直接
            // read/recv()， sockfd是否读就绪，我们不清楚
            // 只有谁最清楚，未来sockfd上是否有事件就绪？select！
            // 将新的sockfd，托管给select！
            // 如何托管? 将新的fd放入辅助数组！
            LOG(LogLevel::INFO) << "get a new link, sockfd: "
                                << sockfd << ", client is: " << client.StringAddr();

            struct epoll_event ev;
            ev.events = EPOLLIN;
            ev.data.fd = sockfd;
            int n = epoll_ctl(_epfd, EPOLL_CTL_ADD, sockfd, &ev);
            if (n < 0)
            {
                LOG(LogLevel::WARNING) << "add listensockfd failed";
            }
            else
            {
                LOG(LogLevel::INFO) << "epoll_ctl add sockfd success: " << sockfd;
            }
        }
    }

    // IO处理器
    void Recver(int sockfd)
    {
        char buffer[1024];
        // 我在这里读取的时候，会不会阻塞?
        ssize_t n = recv(sockfd, buffer, sizeof(buffer) - 1, 0); // recv写的时候有bug吗？
        if (n > 0)
        {
            buffer[n] = 0;
            std::cout << "client say@ " << buffer << std::endl;
        }
        else if (n == 0)
        {
            LOG(LogLevel::INFO) << "clien quit...";
            // 1. 不要让select在关系这个fd了
            // 2. 关闭fd
            int m = epoll_ctl(_epfd, EPOLL_CTL_DEL, sockfd, nullptr);
            if(m > 0)
            {
                LOG(LogLevel::INFO) << "epoll_ctl remove sockfd success: " << sockfd;
            }
            close(sockfd);
        }
        else
        {
            LOG(LogLevel::ERROR) << "recv error";
            // 1. 不要让select在关系这个fd了

            // 2. 关闭fd
            int m = epoll_ctl(_epfd, EPOLL_CTL_DEL, sockfd, nullptr);
            if(m > 0)
            {
                LOG(LogLevel::INFO) << "epoll_ctl remove sockfd success: " << sockfd;
            }
            close(sockfd);
        }
    }
    
    void Stop()
    {
        _isrunning = false;
    }
    ~SelectServer()
    {
        _listensock->Close();
        if (_epfd > 0)
            close(_epfd);
    }

private:
    std::unique_ptr<Socket> _listensock;
    bool _isrunning;
    int _epfd;
    struct epoll_event _revs[size];
};