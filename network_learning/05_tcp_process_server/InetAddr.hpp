#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class InetAddr {
public:

  InetAddr(){
    bzero(&_addr, sizeof(_addr));
    _ip = "";
    _port = 0;
  }
  InetAddr(const struct sockaddr_in &addr) : _addr(addr) {
    _port = ntohs(_addr.sin_port);
    _ip = inet_ntoa(_addr.sin_addr);
  }

  std::string Ip() const { return _ip;}
  uint16_t Port() const { return _port;}
  
  std::string PrintDebug() const {
    return _ip + ':' + std::to_string(_port);
  }

  const struct sockaddr *GetAddr() const {
    return (const struct sockaddr *)&_addr;
  }
  socklen_t GetAddrLen () const {
    return sizeof(_addr);
  }

  bool operator==(const InetAddr &other) const {
    return _ip == other.Ip() && _port == other.Port();
  }

  ~InetAddr(){}
private:
  std::string _ip;
  uint16_t _port;
  struct sockaddr_in _addr;
};