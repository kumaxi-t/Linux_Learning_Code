#pragma once
#include <string>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Protocol.hpp"
#include "Calculate.hpp"


class TcpServer {
private:
  uint16_t _port;
  int _listen_sockfd;

public:

  TcpServer(uint16_t port) : _port(port), _listen_sockfd(-1) {}

  void Init() {
    _listen_sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if(_listen_sockfd < 0) {
      std::cerr << "socket error" << std::endl;
      exit(1);
    }

    // 允许端口复用
    int opt = 1;
    setsockopt(_listen_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));

    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(_port);

    if(::bind(_listen_sockfd, (struct sockaddr *)&local, sizeof(local)) < 0) {
      std::cerr << "bind error" << std::endl;
      exit(2);
    }

    if(::listen(_listen_sockfd, 6) < 0) {
      std::cerr << "listen error" << std::endl;
      exit(3);
    }
  }


  void Loop() {
    while(true) {
      struct sockaddr_in peer;
      socklen_t len = sizeof(peer);

      int sockfd = accept(_listen_sockfd, (struct sockaddr *)&peer, &len);
      
      if(sockfd < 0) continue;

      HandleSession(sockfd);

      ::close(sockfd);
    }
  }

  ~TcpServer() {
    if(_listen_sockfd >= 0) {
      ::close(_listen_sockfd);
    }
  }

private:

  void HandleSession(int sockfd) {
    // 客户端序列化发送数据，服务端反序列化拿到数据，服务端计算，服务端序列化返回结果，客户端反序列化拿到结果
    std::string inbuffer;

    while(true) {
      char buf[64];

      ssize_t n = ::recv(sockfd, buf, sizeof(buf) - 1, 0);
      
      if(n > 0) {
        buf[n] = 0;
        inbuffer += buf;

        std::string package;
        while(Protocol::Decode(inbuffer, &package)) {
          // 此时package里就是要读取的完整的数据

          Protocol::Request req;
          // 反序列化
          req.Deserialize(package);
          // 执行计算并获取结果
          Protocol::Response resp = Business::Execute(req);
          // 序列化计算出来的结果
          std::string resp_json;
          resp.Serialize(&resp_json);
          // 把序列化好的数据打包
          std::string send_pkg = Protocol::Encode(resp_json);
          // 发送给客户端
          ::send(sockfd, send_pkg.c_str(), send_pkg.size(), 0);
        }
      }else if(n == 0) {
        std::cout << "Client exit" << std::endl;
        break;
      }else {
        std::cerr << "recv error" << std::endl;
        break;
      }

    }
  }
};