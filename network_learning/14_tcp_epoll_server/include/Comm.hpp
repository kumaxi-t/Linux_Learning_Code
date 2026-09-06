#pragma once
#include <fcntl.h>
#include <cstdio>
#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>        
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void SetNonBlock(int fd);