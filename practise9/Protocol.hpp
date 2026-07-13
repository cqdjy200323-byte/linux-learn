#pragma once
#include <iostream>
#include <string>
#include <memory>
#include "Socket.hpp"
#include <jsoncpp/json/json.h>
#include "Common.hpp"

using namespace SocketModule;

class Request
{
public:
    Request()
    {
    }

    Request(int x, int y, char oper)
        : _x(x), _y(y), _oper(oper)
    {
    }

    std::string Serialize()
    {
        Json::Value root;
        root["x"] = _x;
        root["y"] = _y;
        root["oper"] = _oper;
        Json::FastWriter Writer;
        std::string s = Writer.write(root);
        return s;
    }

    bool Deserialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool Ok = reader.parse(in, root);
        if (Ok)
        {
            _x = root["x"].asInt();
            _y = root["y"].asInt();
            _oper = root["oper"].asInt();
        }
        return Ok;
    }
    ~Request()
    {
    }
    int X()
    {
        return _x;
    }
    int Y()
    {
        return _y;
    }
    char Oper()
    {
        return _oper;
    }

private:
    int _x;
    int _y;
    char _oper;
};

class Response
{
public:
    Response()
    {
    }

    Response(int result, int code)
        : _result(result), _code(code)
    {
    }

    std::string Serialize()
    {
        Json::Value root;
        Json::FastWriter writer;
        root["result"] = _result;
        root["code"] = _code;
        std::string s = writer.write(root);
        return s;
    }
    bool Deserialize(std::string &in)
    {
        Json::Value root;
        Json::Reader reader;
        bool ok = reader.parse(in, root);
        if (ok)
        {
            _result = root["result"].asInt();
            _code = root["code"].asInt();
        }
        return ok;
    }
    void SetResult(int res)
    {
        _result = res;
    }
    void SetCode(int code)
    {
        _code = code;
    }

    void ShowResult()
    {
        std::cout << "计算结果是: " << _result << "[" << _code << "]" << std::endl;
    }
    ~Response() {}

private:
    int _result;
    int _code;
};

const std::string sep = "\r\n";

using func_t = std::function<Response(Request &req)>;

class Protocol
{
public:

    Protocol()
    {
    }
    
    Protocol(func_t func)
        : _func(func)
    {
    }

    std::string EnCode(const std::string &json_str)
    {
        std::string len = std::to_string(json_str.size());
        return len + sep + json_str + sep;
    }

    bool DeCode(std::string &buffer, std::string *package)
    {
        ssize_t pos = buffer.find(sep);
        if (pos == std::string::npos)
        {
            return false;
        }
        std::string package_len_str = buffer.substr(0, pos);
        int package_len_int = std::stoi(package_len_str);
        int target_len = package_len_str.size() + package_len_int + 2 * sep.size();
        if (buffer.size() < target_len)
            return false;
        *package = buffer.substr(pos + sep.size(), package_len_int);
        buffer.erase(0, target_len);
        return true;
    }
    void GetRequest(std::shared_ptr<Socket> &sock, InetAddr &client)
    {
        std::string buffer_queue;
        while (true)
        {
            int n = sock->Recv(&buffer_queue);
            if (n > 0)
            {
                std::string json_package;
                while (DeCode(buffer_queue, &json_package))
                {
                    LOG(LogLevel::DEBUG) << client.StringAddr() << " 请求: " << json_package;
                    Request req;
                    bool ok = req.Deserialize(json_package);
                    if (!ok)
                        continue;

                    Response resp = _func(req);

                    // 4. 序列化
                    std::string json_str = resp.Serialize();

                    // 5. 添加自定义长度
                    std::string send_str = EnCode(json_str); // 携带长度的应答报文了"len\r\n{result:XXX, code:XX}\r\n"

                    // 6. 直接发送
                    sock->Send(send_str);
                }
            }
            else if (n == 0)
            {
                LOG(LogLevel::INFO) << "client:" << client.StringAddr() << "Quit!";
                break;
            }
            else
            {
                LOG(LogLevel::WARNING) << "client:" << client.StringAddr() << ", recv error";
                break;
            }
        }
    }

    bool GetResponse(std::shared_ptr<Socket> &client, std::string &resp_buff, Response *resp)
    {
        while (true)
        {
            int n = client->Recv(&resp_buff);
            if (n > 0)
            {
                // std::cout << "-----------resp_buffer--------------" << std::endl;
                // std::cout << resp_buff << std::endl;
                // std::cout << "------------------------------------" << std::endl;

                // 成功
                std::string json_package;
                // 1. 解析报文，提取完整的json请求，如果不完整，就让服务器继续读取
                // bool ret = Decode(resp_buff, &json_package);
                // if (!ret)
                //     continue;

                while (DeCode(resp_buff, &json_package))
                {
                    // std::cout << "----------response json---------------" << std::endl;
                    // std::cout << json_package << std::endl;
                    // std::cout << "--------------------------------------" << std::endl;

                    // std::cout << "-----------resp_buffer--------------" << std::endl;
                    // std::cout << resp_buff << std::endl;
                    // std::cout << "------------------------------------" << std::endl;
                    // 2. 走到这里，我能保证，我一定拿到了一个完整的应答json报文
                    // 2. 反序列化
                    resp->Deserialize(json_package);
                }
                return true;
            }
            else if (n == 0)
            {
                std::cout << "server quit " << std::endl;
                return false;
            }
            else
            {
                std::cout << "recv error" << std::endl;
                return false;
            }
        }
    }

    std::string BuildRequestString(int x, int y, char oper)
    {
        Request req(x, y, oper);
        std::string json_req = req.Serialize();
        return EnCode(json_req);
    }
    ~Protocol()
    {
    }

private:
    // 因为我们用的是多进程
    // Request _req;
    // Response _resp;
    func_t _func;
};