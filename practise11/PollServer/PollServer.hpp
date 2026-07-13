#pragma once

#include <iostream>
#include <memory>
#include <unistd.h>
#include <sys/poll.h>
#include "Socket.hpp"
#include "Log.hpp"


using namespace LogModule;
using namespace SocketModule;
class SelectServer
{
    const static int size = 4096;
    const static int defaultfd = -1;

public:
    SelectServer(int port)
        : _listensock(std::make_unique<TcpSocket>()), _isrunning(false)
    {
        _listensock->BuildTcpSocketMethod(port);
        for (int i = 0; i < size;i++)
        {
            _fds[i].fd = defaultfd;
            _fds[i].events = 0;
            _fds[i].revents = 0;
        }
        _fds[0].fd = _listensock->Fd();
        _fds[0].events = POLLIN;

    }

    void Start()
    {
        _isrunning = true;
        int timeout = -1;
        while (_isrunning)
        {
            PrintFd();
            int n = poll(_fds, size, timeout);
            switch (n)
            {
            case -1:
                LOG(LogLevel::ERROR) << "poll error";
                break;
            case 0:
                LOG(LogLevel::INFO) << " polltime out...";
                break;
            default:
                // 有事件就绪,就不仅仅是新连接到来了吧？读事件就绪啊？
                LOG(LogLevel::DEBUG) << "有事件就绪了..., n : " << n;
                Dispatcher(); // 处理就绪的事件啊！
                break;
            }
        }

        _isrunning = false;
    }

    void Dispatcher()
    {
        for (int i = 0; i < size; i++)
        {
            if (_fds[i].fd == defaultfd)
                continue;
            // fd合法，不一定就绪
            if (_fds[i].revents & POLLIN)
            {
                // fd_array[i] 上面一定是读就绪了
                // listensockfd 新连接到来，也是读事件就绪啊
                // sockfd 数据到来，读事件就绪啊
                if (_fds[i].fd == _listensock->Fd())
                {
                    // listensockfd 新连接到来
                    Accepter();
                }
                else
                {
                    // 普通的读事件就绪
                    Recver(i);
                }
            }

            // if (FD_ISSET(fd_array[i], &wfds))
            // {
            //     // fd_array[i] 上面一定是读就绪了
            // }
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
            int pos = 0;
            for (; pos < size; pos++)
            {
                if (_fds[pos].fd == defaultfd)
                    break;
            }
            if (pos == size)
            {
                LOG(LogLevel::WARNING) << "select server full";
                close(sockfd);
            }
            else
            {
                _fds[pos].fd = sockfd;
                _fds[pos].events = POLLIN;
                _fds[pos].revents = 0;
            }
        }
    }

    // IO处理器
    void Recver(int pos)
    {
        char buffer[1024];
        // 我在这里读取的时候，会不会阻塞?
        ssize_t n = recv(_fds[pos].fd, buffer, sizeof(buffer) - 1, 0); // recv写的时候有bug吗？
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
            close(_fds[pos].fd);
            _fds[pos].fd = defaultfd;
            _fds[pos].events = 0;
            _fds[pos].revents = 0;
        }
        else
        {
            LOG(LogLevel::ERROR) << "recv error";
            // 1. 不要让select在关系这个fd了

            // 2. 关闭fd
            close(_fds[pos].fd);
            _fds[pos].fd = defaultfd;
            _fds[pos].events = 0;
            _fds[pos].revents = 0;
        }
    }
    void PrintFd()
    {
        std::cout << "_fd_array[]: ";
        for (int i = 0; i < size; i++)
        {
            if (_fds[i].fd == defaultfd)
                continue;
            std::cout << _fds[i].fd << " ";
        }
        std::cout << "\r\n";
    }
    void Stop()
    {
        _isrunning = false;
    }
    ~SelectServer()
    {
    }

private:
    std::unique_ptr<Socket> _listensock;
    bool _isrunning;
    struct pollfd _fds[size];
};