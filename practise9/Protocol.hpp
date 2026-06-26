#pragma once
#include <iostream>
#include <string>
#include <memory>
#include "Socket.hpp"


using namespace SocketModule;


class Request
{
public:
    Request()
    {}

    Request(int x,int y, char oper)
    :_x(x)
    ,_y(y)
    ,_oper(oper)
    {

    }

    std::string Serialize()
    {

    }

    ~Request()
    {}



private:
    int _x;
    int _y;
    char _oper;
};


class Response
{
    Response()
    {}

    Response(int result,int code)
    :_result(result)
    ,_code(code)
    {}

    bool Deserialize(std::string &in)
    {
    }
    ~Response() {}



private:
    int _result;
    int _code;
};

class Protocol
{
public:
    Protocol()
    {}
    void GetRequest(std::shared_ptr<Socket> &sock, InetAddr &client)
    {

    }
    ~Protocol()
    {}
private:
    // 因为我们用的是多进程
    // Request _req;
    // Response _resp;
};