#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <functional>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
enum ExitCode{
    OK = 0,
    USAGE_ERR,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    CONNECT_ERR,
    FORK_ERR,
    OPEN_ERR
};

class nocopy {
public:
  nocopy() {}
  ~nocopy() {}
  nocopy(const nocopy &) = delete;
  nocopy& operator=(const nocopy &) = delete;

};

#define CONV(addr_ptr) ((struct sockaddr *) addr_ptr)