.PHONY: all clean

BIN1=01_cond_base
BIN2=02_cond_broadcast

CC=g++
FLAGS=-std=c++11 -Wall
LDFLAGS=-lpthread

all: $(BIN1) $(BIN2)

$(BIN1): 01_cond_base.cpp
	$(CC) -o $@ $^ $(FLAGS) $(LDFLAGS)

$(BIN2): 02_cond_broadcast.cpp
	$(CC) -o $@ $^ $(FLAGS) $(LDFLAGS)

clean:
	rm -f $(BIN1) $(BIN2)