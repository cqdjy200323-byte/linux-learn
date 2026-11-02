#include <iostream>
#include <string>
#include "Listener.hpp"
#include "TcpServer.hpp"
#include "Log.hpp"
#include "Common.hpp"

static void Usage(std::string proc)
{
    std::cerr << "Usage: " << proc << " port" << std::endl;
}

//./server port
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    LogModule::ConsoleLogStrategy();
    uint16_t port = std::stoi(argv[1]);

    std::shared_ptr<Connection> conn = std::make_shared<Listener>(port);
    std::unique_ptr<Reactor> tsvr = std::make_unique<Reactor>();
    tsvr->AddConnection(conn);

    return 0;   
}