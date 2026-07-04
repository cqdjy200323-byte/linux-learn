#include "Socket.hpp"
#include"TcpServer.hpp"
#include <unistd.h>
#include"Http.hpp"

void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    uint16_t port = std::stoi(argv[1]);
    std::unique_ptr<Http> httpsvr = std::make_unique<Http>(port);
    httpsvr->Start();

    return 0;
}