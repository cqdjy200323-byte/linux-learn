#include <sys/socket.h>
#include<iostream>
#include<string.h>
#include <arpa/inet.h>
#include<string>
#include <unistd.h>

class UdpServer
{
public:
    UdpServer(int port,std::string ip)
    :_sockfd(-1)
    ,_port(port)
    ,_ip(ip)
    {
    }
    bool ServerInit()
    {
        _sockfd=socket(AF_INET,SOCK_DGRAM,0);
        if(_sockfd<0)
        {
            std::cerr << "socket fail" << std::endl;
            return false;
        }
        std::cout << "socket create success, sockfd: " << _sockfd << std::endl;

        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family=AF_INET;
        local.sin_port = htons(_port);
        inet_pton(AF_INET, _ip.c_str(), &local.sin_addr);
        if(bind(_sockfd,(struct sockaddr*)&local,sizeof(sockaddr))<0)
        {
            std::cerr << "bind fail" << std::endl;
            return false;
        }
        std::cout << "bind success" << std::endl;
        return true;
    }


    void Start()
    {
#define SIZE 128
        char buffer[SIZE];
        for (;;)
        {
            struct sockaddr_in peer;
            socklen_t len = sizeof(peer);
            ssize_t size = recvfrom(_sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
            if(size>0)
            {
                buffer[size] = '\0';
                int port = ntohs(peer.sin_port);
                std::string ip = inet_ntoa(peer.sin_addr);
                std::cout << ip << ":" << port << "# " << buffer << std::endl;
            }
            else
            {
                std::cerr << "recvfrom error" << std::endl;
            }
        }
    }
    ~UdpServer()
    {
        if(_sockfd>0)
        {
            close(_sockfd);
        }
    }

private:
    int _sockfd;
    int _port;
    std::string _ip;
};


int main(int argc,char* argv[])
{
    if(argc!=2)
    {
        std::cout << "please enter:" << argv[0] << " " << "port" << std::endl;
    }
    std::string ip = "127.0.0.01"; // 本地环回
    int port = atoi(argv[1]);
    UdpServer *svr = new UdpServer(port,ip);
    svr->ServerInit();
    svr->Start();
    return 0;
}