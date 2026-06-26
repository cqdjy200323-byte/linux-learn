.PHONY:all
all:tcpclient tcpserver

tcpclient:TcpClient.cc
	g++ -o $@ $^ -std=c++17 -lpthread -static
tcpserver:TcpServer.cc
	g++ -o $@ $^ -std=c++17

.PHONY:clean
clean:
	rm -f tcpclient tcpserver