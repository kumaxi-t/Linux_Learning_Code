#pragma once
#include <string>

struct Connection {
  int _sock_fd;
  std::string _inbuffer;
  std::string _outbuffer;
  explicit Connection(int sock_fd) : _sock_fd(sock_fd) {}

};