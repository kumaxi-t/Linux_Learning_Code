#pragma once
#include <iostream>
#include <cstdint>
#include <unistd.h>
#include "nocopy.hpp"
#include "Comm.hpp"

static const int defaultsocket = -1;
static const int defaultbacklog = 6;

class TcpServer : public nocopy{
public:
  TcpServer(uint16_t port) : _port(port), _listensock(defaultsocket) {

  }

  void Init() {
    _listensock = socket(AF_INET, SOCK_STREAM, 0);
    if(_listensock < 0) {
      std::cerr << "creat sock error" << std::endl;
      exit(Socket_Err);
    }

    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(_port);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(_listensock, CONV(&local), sizeof(local)) != 0) {
      std::cerr << "bind socket error" << std::endl;
      exit(Bind_Err);
    }

    if(listen(_listensock, defaultbacklog) != 0) {
      std::cerr << "listen socket error" << std::endl;
      exit(Listen_Err);
    }
    
  }

  void Start() {
    while(true) {
      struct sockaddr_in peer;
      socklen_t len = sizeof(peer);

      int sockfd = accept(_listensock, (struct sockaddr *)&peer, &len);
      if(sockfd < 0) {
        std::cerr << "accept error, continue next" << std::endl;
        continue;
      }
      Server(sockfd);
      close(sockfd);
    }
  }


  void Server(int sockfd) {
    char buffer[64];
    while(true) {
      ssize_t n = read(sockfd, buffer, sizeof(buffer) - 1);
      if(n > 0) {
        // 成功读取到数据
        buffer[n] = 0;
        std::cout << "client say# " << buffer << std::endl;

        std::string echo_string = "server echo# ";
        echo_string += buffer;
        write(sockfd, echo_string.c_str(), echo_string.size()); 
      }else if(n == 0) {
        // 对端调用close关闭连接
        std::cout << "client quit, server close sockfd :" << sockfd << std::endl;
        break;
      }else {
        std::cerr << "read socked error" << std::endl;
        break;
      }
    }
  }

  ~TcpServer() {

  }

private:
  uint16_t _port;
  int _listensock;

};