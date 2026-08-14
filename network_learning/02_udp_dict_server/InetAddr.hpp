#pragma once
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class InetAddr {
public:
  InetAddr(const struct sockaddr_in &addr) : _addr(addr) {
    _port = ntohs(_addr.sin_port);
    _ip = inet_ntoa(_addr.sin_addr);
  }

  std::string Ip() const { return _ip;}
  uint16_t Port() const { return _port;}
  
  std::string PrintDebug() const {
    return _ip + ':' + std::to_string(_port);
  }

  ~InetAddr(){}
private:
  std::string _ip;
  uint16_t _port;
  struct sockaddr_in _addr;
};