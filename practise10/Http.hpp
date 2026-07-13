#include <iostream>
#include "TcpServer.hpp"
#include "Util.hpp"

using namespace SocketModule;
using namespace LogModule;

const std::string gspace = " ";
const std::string glinespace = "\r\n";
const std::string glinesep = ": ";

const std::string webroot = "./wwwroot";
const std::string homepage = "index.html";
const std::string page_404 = "/404.html";

class HttpRequest
{
public:
    HttpRequest()
    {
    }

    ~HttpRequest()
    {
    }

    void ParseReqLine(std::string &reqline)
    {
        std::stringstream ss(reqline);
        ss >> _method >> _uri >> _version;
    }
    std::string Serialize()
    {
        return std::string();
    }

    bool DeSerialize(std::string reqstr)
    {
        std::string reqline;
        int n = Util::ReadOneLine(reqstr, &reqline, glinespace);
        LOG(LogLevel::DEBUG) << reqline;
        ParseReqLine(reqline);
        LOG(LogLevel::DEBUG) << "_method: " << _method;
        LOG(LogLevel::DEBUG) << "_uri: " << _uri;
        LOG(LogLevel::DEBUG) << "_version: " << _version;
        if (_uri == "/")
            _uri = webroot + _uri + homepage; // ./wwwroot/index.html
        else
            _uri = webroot + _uri; // ./wwwroot/a/b/c.html
        return true;
    }
    std::string Uri()
    {
        return _uri;
    }

private:
    std::string _method;
    std::string _uri;
    std::string _version;
    std::unordered_map<std::string, std::string> _headers;
    std::string _blankline;
    std::string _text;
};

class HttpResponse
{
public:
    HttpResponse() : _blankline(glinespace), _version("HTTP/1.0")
    {
    }

    ~HttpResponse()
    {
    }

    std::string Serialize()
    {
        std::string status_line = _version + gspace + std::to_string(_code) + gspace + _desc + glinespace;
        std::string respheader;
        for (auto &h : _headers)
        {
            std::string line = h.first + glinesep + h.second + glinespace;
            respheader += line;
        }
        return status_line + respheader + _blankline + _text;
    }

    void SetCode(int code)
    {
        _code = code;
        switch (_code)
        {
        case 200:
            _desc = "OK";
            break;
        case 404:
            _desc = "Not Found";
            break;
        default:
            break;
        }
    }

    std::string Uri2Suffix(const std::string &targetfile)
    {
        // ./wwwroot/a/b/c.html
        auto pos = targetfile.rfind(".");
        if (pos == std::string::npos)
        {
            return "text/html";
        }

        std::string suffix = targetfile.substr(pos);
        if (suffix == ".html" || suffix == ".htm")
            return "text/html";
        else if (suffix == ".jpg")
            return "image/jpeg";
        else if (suffix == ".png")
            return "image/png";
        else
            return "";
    }
    void SetHeader(const std::string &key, const std::string &value)
    {
        auto iter = _headers.find(key);
        if (iter != _headers.end())
            return;
        _headers.insert(std::make_pair(key, value));
    }

    bool MakeResponse()
    {
        // 清空旧头部，防止上次请求残留
        _headers.clear();
        _text.clear();

        if (_targetfile == "./wwwroot/favicon.ico")
        {
            LOG(LogLevel::DEBUG) << "用户请求: " << _targetfile << "忽略它";
            SetCode(200);
            SetHeader("Content-Length", "0");
            return true;
        }

        bool res = Util::ReadFileContent(_targetfile, &_text);
        if (!res)
        {
            LOG(LogLevel::WARNING) << "client want get : " << _targetfile << " but not found";
            SetCode(404);
            _targetfile = webroot + page_404;
            Util::ReadFileContent(_targetfile, &_text);
        }
        else
        {
            LOG(LogLevel::DEBUG) << "读取文件: " << _targetfile;
            SetCode(200);
        }

        // 直接取内存字符串长度，不再读取磁盘文件大小
        std::string suffix = Uri2Suffix(_targetfile);   
        SetHeader("Content-Type", suffix);
        SetHeader("Content-Length", std::to_string(_text.size()));
        return true;
    }

    void SetTargetFile(const std::string &target)
    {
        _targetfile = target;
    }
    bool DeSerialize()
    {
        return true;
    }

public:
    std::string _version;
    int _code;         // 404
    std::string _desc; // "Not Found"

    std::unordered_map<std::string, std::string> _headers;
    std::string _blankline;
    std::string _text;

    std::string _targetfile;
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
        int n = sock->Recv(&httpreqstr);
        if (n > 0)
        {
            HttpRequest req;
            req.DeSerialize(httpreqstr);
            HttpResponse resp;
            resp.SetTargetFile(req.Uri());
            resp.MakeResponse();
            std::string response_str = resp.Serialize();
            sock->Send(response_str);
        }
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