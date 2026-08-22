.PHONY: all clean

CC = g++
CFLAGS = -Wall -std=c++14
LDFLAGS = -ljsoncpp

ALL_BIN = tcp_server tcp_client

all: $(ALL_BIN)

tcp_server: TcpServerMain.cc
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tcp_client: TcpClientMain.cc
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(ALL_BIN)