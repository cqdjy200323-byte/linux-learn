#include <iostream>
#include "TcpServer.hpp"


using namespace SocketModule;
using namespace LogModule;

const std::string gspace = " ";
const std::string glinespace = "\r\n";
const std::string glinesep = ": ";


class HttpRequest
{
public:
    HttpRequest()
    {}

    ~HttpRequest()
    {

    }

    std::string Serlalize()
    {
        return std::string();
    }

    bool DeSerlalize()
    {
        return true;
    }

private:
    std::string _method;
    std::string _uri;
    std::string _version;
    std::unordered_map<std::string, std::string> _headers;
    std::string _blankline;
    std::string _text;
};

class Http
{
public:
    Http(uint16_t port)
        : tsvrp(std::make_unique<TcpServer>(port))
    {
    }
    ~Http()
    {
    }

    void HandlerHttpRquest(std::shared_ptr<Socket> &sock, InetAddr &client)
    {
        std::string httpreqstr;
        sock->Recv(&httpreqstr);
        std::cout << httpreqstr << std::endl;

#ifdef DEBUG
#endif
    }
    void Start()
    {
        tsvrp->Start(
            [this](std::shared_ptr<Socket> &sock, InetAddr &client)
            { this->HandlerHttpRquest(sock, client); });
    }

private:
    std::unique_ptr<TcpServer> tsvrp;
};