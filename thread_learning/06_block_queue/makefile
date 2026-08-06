.PHONY: all clean

BIN=block_queue
CC=g++
FLAGS=-std=c++11 -Wall
LDFLAGS=-lpthread

$(BIN): Main.cc
	$(CC) -o $@ $^ $(FLAGS) $(LDFLAGS)

clean:
	rm -f $(BIN)