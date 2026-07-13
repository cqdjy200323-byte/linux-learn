.PHONY:all
all:ServerNetCald client_netcal 

ServerNetCald:main.cc
	g++ -o $@ $^ -std=c++17 -ljsoncpp -lpthread
client_netcal:TcpClient.cc
	g++ -o $@ $^ -std=c++17 -ljsoncpp -lpthread



.PHONY:output
output:
	@mkdir output
	@mkdir -p output/bin
	@mkdir -p output/conf
	@mkdir -p output/log
	@cp ServerNetCald output/bin
	@cp client_netcal output/bin
	@cp test.conf output/conf
	@cp install.sh output/
	@cp uninstall.sh output/
	@tar czf output.tgz output


.PHONY:clean
clean:
	rm -rf ServerNetCald client_netcal output output.tgz