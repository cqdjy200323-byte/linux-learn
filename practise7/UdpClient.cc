#include <iostream>
#include <string>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "Thread.hpp"

int sockfd = 0;
std::string server_ip;
uint16_t server_port = 0;
pthread_t id;

using namespace ThreadModlue;

void Recv()
{
    while (true)
    {
        char buffer[1024];
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int m = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);
        if (m > 0)
        {
            buffer[m] = 0;
            std::cerr << buffer << std::endl;
        }
    }
}

void Send()
{
    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());
    const std::string online = "inline";
    sendto(sockfd, online.c_str(), online.size(), 0, (struct sockaddr *)&server, sizeof(server));

    while (true)
    {
        std::string input;
        std::cout << "Please Enter# "; // 1
        std::getline(std::cin, input); // 0

        int n = sendto(sockfd, input.c_str(), input.size(), 0, (struct sockaddr *)&server, sizeof(server));
        (void)n;
        if (input == "QUIT")
        {
            pthread_cancel(id);
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " server_ip server_port" << std::endl;
        return 1;
    }
    server_ip = argv[1];
    server_port = std::stoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        return 2;
    }

    Thread Recver(Recv);
    Thread Sender(Send);

    Recver.Start();
    Sender.Start();

    Recver.Join();
    Sender.Join();

    return 0;
}