.PHONY:all
all:tcp_server tcp_client

tcp_server:ServerMain.cc
	g++ -o $@ $^ -std=c++14

tcp_client:TcpClient.cc
	g++ -o $@ $^ -std=c++14

.PHONY:clean
clean:
	rm -f tcp_server tcp_client