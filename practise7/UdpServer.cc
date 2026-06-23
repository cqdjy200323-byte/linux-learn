#include <iostream>
#include <memory>
#include "Route.hpp"
#include "UdpServer.hpp" // 网络通信的功能
#include "ThreadPool.hpp"

using namespace ThreadPoolModule;

using task_t = std::function<void()>;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " port" << std::endl;
        return 1;
    }

    uint16_t port = std::stoi(argv[1]);
    Enable_Console_Log_Strategy();

    Route r;

    auto tp = ThreadPool<task_t>::GetInstance();

    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(
        port,
        [&r, &tp](int sockfd, const std::string &message, InetAddr &peer)
        {
            task_t t = std::bind(&Route::MessageRoute, &r, sockfd, message, peer);
            tp->Enqueue(t);
        });

    usvr->Init();
    usvr->Start();

    return 0;
}