#include "SelectServer.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " port" << std::endl;
        exit(USAGE_ERR);
    }
    Enable_Console_Log_Strategy();
    uint16_t port = std::stoi(argv[1]);

    std::unique_ptr<SelectServer> svr = std::make_unique<SelectServer>(port);
    svr->Start();

    return 0;
}