#pragma once

#include <iostream>
#include <sys/types.h>         
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <functional>
#include <sys/epoll.h>
#include <memory>
#include <sstream>
#include <fstream>


bool SetNonBlock(int fd);


std::string ReadFile(const std::string& path);