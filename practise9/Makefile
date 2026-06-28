.PHONY:all
all:tcpclient tcpserver

tcpclient:TcpClient.cc
	g++ -o $@ $^ -std=c++17 -ljsoncpp
tcpserver:main.cc
	g++ -o $@ $^ -std=c++17 -ljsoncpp

.PHONY:clean
clean:
	rm -f tcpclient tcpserver