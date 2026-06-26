#include "Socket.hpp"
#include "Protocol.hpp"

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

    std::unique_ptr<Protocol> protocol = std::make_unique<Protocol>();

    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        std::stoi(argv[1]),
        [&protocol](std::shared_ptr<Socket> &sock, InetAddr &client)
        {
            protocol->GetRequest(sock, client);
        });
    tsvr->Start();

    return 0;
}
