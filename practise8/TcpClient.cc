#include <iostream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "InetAddr.hpp"
#include "common.hpp"

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " server_ip server_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    std::string serverip = argv[1];
    uint16_t serverport = std::stoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(SOCKET_ERR);
    }

    InetAddr serveraddr(serverip, serverport);
    int n = connect(sockfd, serveraddr.NetAddrPtr(), serveraddr.NetAddrLen());
    if (n < 0)
    {
        std::cerr << "connect error" << std::endl;
        exit(CONNECT_ERR);
    }

    while(true)
    {
        std::string line;
        std::cout << "Please Enter@ ";
        std::getline(std::cin, line);

        write(sockfd, line.c_str(), line.size());

        char buffer[1024];
        ssize_t size = read(sockfd, buffer, sizeof(buffer)-1);
        if(size > 0)
        {
            buffer[size] = 0;
            std::cout << "server echo# " << buffer << std::endl;
        }
    }

    close(sockfd);

    return 0;
}