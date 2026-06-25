#include <iostream>
#include <memory>
#include "TcpServer.hpp" // 网络通信的功能



int main(int argc, char* argv[])
{
    if(argc!=2)
    {
        std::cerr << argv[1]<<std::endl;
    }

    uint16_t port = std::stoi(argv[1]);

    Enable_Console_Log_Strategy();
    
    
}