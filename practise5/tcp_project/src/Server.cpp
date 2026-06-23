#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>

#define BACKLOG 5
class TcpServer
{
public:
    TcpServer(int port)
        : _sockfd(-1), _port(port)
    {
    }
    ~TcpServer()
    {
        if (_sockfd > 0)
        {
            close(_sockfd);
        }
    }

    void TcpInit()
    {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_sockfd < 0)
        {
            std::cerr << "socket fail" << std::endl;
            exit(2);
        }

        struct sockaddr_in local;
        memset(&local,0,sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;

        if(bind(_sockfd,(struct sockaddr*)&local,sizeof(local))<0)
        {
            std::cerr << "bind fail" << std::endl;
            exit(3);
        }
        if(listen(_sockfd,BACKLOG)<0)
        {
            std::cerr << "listen error" << std::endl;
			exit(4);
        }
    }

    void Start()
    {
        for (;;)
        {
            struct sockaddr_in peer;
            memset(&peer, 0, sizeof(peer));
            socklen_t len = sizeof(peer);
            int sock=accept(_sockfd,(struct sockaddr*)&peer,&len);
            if(sock<0)
            {
                std::cerr << "accept error, continue next" << std::endl;
				continue;
            }
            std::string client_ip = inet_ntoa(peer.sin_addr);
            int client_port = ntohs(peer.sin_port);
            std::cout<<"get a new link->"<<sock<<" ["<<client_ip<<"]:"<<client_port<<std::endl;
            Service(sock, client_ip, client_port);
        }
    }

    void Service(int sock,std::string client_ip,int client_port)
    {
#define SIZE 1024
        char buffer[SIZE];
        while(true)
        {
            ssize_t size=read(sock,buffer,sizeof(buffer)-1);
            if(size>0)
            {
                buffer[size]='\0';
                std::cout << "get a new link->" << sock << " [" << client_ip << "]:" << client_port << std::endl;
                std::cout << buffer << std::endl;
                write(sock, buffer, size);
            }
            else if(size==0)
            {
                std::cout << client_ip << ":" << client_port << " close!" << std::endl;
				break;
            }
            else
            {
                std::cerr << sock << " read error!" << std::endl;
				break;
            }
        }
        close(sock);
        std::cout << client_ip << ":" << client_port << " service done!" << std::endl;
    }

private:
    int _sockfd;
    int _port;
};


void Usage(std::string proc)
{
	std::cout << "Usage: " << proc << " port" << std::endl;
}
int main(int argc, char* argv[])
{
	if (argc != 2){
		Usage(argv[0]);
		exit(1);
	}
	int port = atoi(argv[1]);
	TcpServer* svr = new TcpServer(port);
	svr->TcpInit();
	svr->Start();
	return 0;
}