#include "Socket.hpp"
#include "Protocol.hpp"
#include"TcpServer.hpp"
#include"NetCal.hpp"
#include <unistd.h>
using namespace SocketModule;
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

    std::cout << "服务器已经启动，已经是一个守护进程了" << std::endl;

    daemon(0,0);

    Enable_File_Log_Strategy();

    std::unique_ptr<Cal> nt = std::make_unique<Cal>();

    std::unique_ptr<Protocol> protocol = std::make_unique<Protocol>(
        [&nt](Request& req)->Response
        {
            return nt->Execute(req);
        });

    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        std::stoi(argv[1]),
        [&protocol](std::shared_ptr<Socket> &sock, InetAddr &client)
        {
            protocol->GetRequest(sock, client);
        });

    tsvr->Start();

    return 0;
}
