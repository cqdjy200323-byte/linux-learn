#include "Socket.hpp"
#include <memory>
#include <iostream>
#include <sys/wait.h>
#include <functional>
#include"Common.hpp"

using namespace SocketModule;
using namespace LogModule;

using ioservice_t=std::function<void(std::shared_ptr<Socket>& sock,InetAddr& client)>;


class TcpServer
{
public:
    TcpServer(uint16_t port,ioservice_t service)
        : _port(port)
        , _isrunning(false)
        ,_service(service)
        ,_listensocket(std::make_unique<TcpSocket>())
    {
        _listensocket->BuildTcpSocketMethod(_port);
    }

    void Start()
    {
        _isrunning = true;
        while (_isrunning)
        {
            InetAddr client;
            auto sock = _listensocket->Accept(&client);
            if (sock == nullptr)
            {
                continue;
            }
            LOG(LogLevel::DEBUG) << "accept success ...";

            pid_t id = fork();

            if(id>0)
            {
                sock->Close();
                pid_t rid = waitpid(id, nullptr, 0);
                (void)rid;
            }
            else if(id==0)
            {
                _listensocket->Close();
                if(fork()>0)
                {
                    exit(OK);
                }
                
                _service(sock, client);
                exit(OK);
            }
            else
            {
                LOG(LogLevel::FATAL) << "fork error ...";
                exit(FORK_ERR);
            }

        }
        _isrunning = false;
    }

    ~TcpServer()
    {
    }

private:
    uint16_t _port;
    bool _isrunning;
    std::unique_ptr<Socket> _listensocket;

    ioservice_t _service;
};