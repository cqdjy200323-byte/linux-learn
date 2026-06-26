#include <iostream>
#include <memory>
#include "TcpServer.hpp" // 网络通信的功能
#include "Command.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << argv[1] << std::endl;
    }

    uint16_t port = std::stoi(argv[1]);

    Enable_Console_Log_Strategy();

    Command cmd;

    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(
        port,
        std::bind(&Command::Execute, &cmd, std::placeholders::_1, std::placeholders::_2));
    // std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(port, [&cmd](const std::string &command, InetAddr &addr)
    //                                                               { return cmd.Execute(command, addr); });

    // std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(port, [&d](const std::string &word, InetAddr &addr){
    //     return d.Translate(word, addr);
    // });
    tsvr->Init();
    tsvr->Run();
}