#include <iostream>
#include <unistd.h>
#include <queue>
#include <pthread.h>
#include <string>    
#include <cstring>    
#include <arpa/inet.h>
#include"ThreadPool.h"


class Handler
{
public:
    Handler()
    {
    }

    ~Handler()
    {
    }

    void operator()(int sock, std::string client_ip, int client_port)
    {
        char buffer[1024];
        while (true)
        {
            ssize_t size = read(sock, buffer, sizeof(buffer) - 1);
            if (size > 0)
            { // 读取成功
                buffer[size] = '\0';
                std::cout << client_ip << ":" << client_port << "# " << buffer << std::endl;

                write(sock, buffer, size);
            }
            else if (size == 0)
            { // 对端关闭连接
                std::cout << client_ip << ":" << client_port << " close!" << std::endl;
                break;
            }
            else
            { // 读取失败
                std::cerr << sock << " read error!" << std::endl;
                break;
            }
        }
        close(sock); // 归还文件描述符
        std::cout << client_ip << ":" << client_port << " service done!" << std::endl;
    }
};

class Task
{
public:
    Task()
    {
    }

    Task(int sock, std ::string ip, int port)
        : _sock(sock), _client_ip(ip), _client_port(port)
    {
    }
    ~Task()
    {
    }
    void Run()
    {
        _handler(_sock, _client_ip, _client_port);
    }

private:
    int _sock;
    std::string _client_ip;
    int _client_port;
    Handler _handler;
};