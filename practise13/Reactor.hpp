#include <iostream>
#include <memory>
#include <unordered_map>
#include "Epoller.hpp"
#include "Connection.hpp"

class Reactor
{
private:
    bool IsConnectionExistsHelper(int sockfd)
    {
        auto iter = _connections.find(sockfd);
        if (iter == _connections.end())
            return false;
        else
            return true;
    }
    bool IsConnectionExists(const std::shared_ptr<Connection> &conn)
    {
        return IsConnectionExistsHelper(conn->GetSockFd());
    }
    bool IsConnectionExists(int sockfd)
    {
        return IsConnectionExistsHelper(sockfd);
    }
    bool IsConnectionEmpty()
    {
        return _connections.empty();
    }

public:
    Reactor()
        : _epoller_ptr(std::make_unique<Epoller>()), _isrunning(false)
    {
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            _epoller_ptr->Wait();
        }

        _isrunning = false;
    }

    void AddConnection(std::shared_ptr<Connection> &conn)
    {
        if(!IsConnectionExists(conn))
        {
            LOG(LogLevel::WARNING) << "conn is exists: " << conn->GetSockFd();
            return;
        }
        uint32_t event = conn->GetEvent();
        int sockfd = conn->GetSockFd();
        _epoller_ptr->AddEvent(sockfd, event);
        conn->SetOwner(this);
        _connections[sockfd] = conn;
    }
    ~Reactor()
    {
    }

private:
    bool _isrunning;
    // 1.epoll 模型
    std::unique_ptr<Epoller> _epoller_ptr;

    // 2.管理所有的链接也就是fd
    std::unordered_map<int, std::shared_ptr<Connection>> _connections;
};