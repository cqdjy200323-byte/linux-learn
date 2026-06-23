#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

class UdpClient
{
public:
    UdpClient(std::string server_ip, int server_port)
        : _sockfd(-1), _server_port(server_port), _server_ip(server_ip)
    {
    }

    bool InitClient()
    {
        // 创建套接字
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0)
        {
            std::cerr << "socket create error" << std::endl;
            return false;
        }
        return true;
    }
    ~UdpClient()
    {
        if (_sockfd >= 0)
        {
            close(_sockfd);
        }
    }

    void Start()
    {
        std::string msg;
        struct sockaddr_in peer;
        memset(&peer, 0, sizeof(peer));
        peer.sin_family = AF_INET;
		peer.sin_port = htons(_server_port);
        inet_pton(AF_INET, _server_ip.c_str(), &peer.sin_addr);

        for (;;)
        {
            std::cout << "Please Enter#";
            getline(std::cin, msg);
            sendto(_sockfd, msg.c_str(), msg.size(), 0, (struct sockaddr *)&peer, sizeof(peer));
        }
    }

private:
    int _sockfd;            // 文件描述符
    int _server_port;       // 服务端端口号
    std::string _server_ip; // 服务端IP地址
};

int main(int argc, char* argv[])
{
	if (argc != 3){
		std::cerr << "Usage: " << argv[0] << " server_ip server_port" << std::endl;
		return 1;
	}
	std::string server_ip = argv[1];
	int server_port = atoi(argv[2]);
	UdpClient* clt = new UdpClient(server_ip, server_port);
	clt->InitClient();
	clt->Start();
	return 0;
}