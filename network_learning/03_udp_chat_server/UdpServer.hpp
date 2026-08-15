#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "InetAddr.hpp"
#include <vector>
#include <sstream>


const static uint16_t defaultport = 8888;
const static int defaultfd = -1;


class UdpServer {
public:
  UdpServer(uint16_t port)
  : _port(port),
    _sockfd(defaultfd),
    _isrunning(false) {}

  ~UdpServer(){
    if(_sockfd >= 0) {
      close(_sockfd);
    }
  }

  void Init() {
    _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_sockfd < 0) {
      std::cerr << "socket error: " << strerror(errno) << std::endl;
      exit(1);
    }

    struct sockaddr_in local;
    bzero(&local, sizeof(local));
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_family = AF_INET;
    local.sin_port = htons(_port);

    int n = bind(_sockfd, (struct sockaddr *)&local, sizeof(local));
    if (n < 0) {
      std::cerr << "bind error: " << strerror(errno) << std::endl;
      exit(2);
    }
    std::cout << "UdpServer 初始化成功，正在监听窗口： " << _port << std::endl;
  }

  void Start() {
    _isrunning = true;
    char buffer[1024];  
    
    while (_isrunning) {
      struct sockaddr_in peer;
      socklen_t len = sizeof(peer);

      // 接收数据
      ssize_t n = recvfrom(_sockfd, buffer, sizeof(buffer) - 1, 0,
                           (struct sockaddr *)&peer, &len);

      if (n > 0) {
        buffer[n] = '\0';
        std::string req = buffer;

        // 去除可能携带的换行符
        if (!req.empty() && req.back() == '\n')
          req.pop_back();
        if (!req.empty() && req.back() == '\r')
          req.pop_back();

        InetAddr who(peer);
        // 先判断是否有用户下线
        if(req == "QUIT" || req == "Q" || req == "quit" || req == "q") {
          RemoveUser(who);
          Broadcast("【已离开聊天室】", who);
          continue;         // 不进入下面的代码，也即不向其他人发送退出信息
        }

        CheckUser(who);

        Broadcast(req, who);
      }
    }
  }

private:
  void CheckUser(const InetAddr &user) {
    for (auto &u : _online_users) {
      if(u == user) {
        return ;
      }
    }
    _online_users.push_back(user);
    std::cout << "新用户上线 ： " << user.PrintDebug() << "当前在线人数： " << _online_users.size() << std::endl;
  }

  void Broadcast(const std::string &message, const InetAddr &who) {
    std::stringstream ss;
    ss << "[" << who.PrintDebug() << "]#" << message << "\n";
    std::string send_message = ss.str();
    for(const auto &u : _online_users) {
      sendto(_sockfd, send_message.c_str(), send_message.size(), 0, u.GetAddr(), u.GetAddrLen());
    }
  }

  void RemoveUser(const InetAddr &user) {
    for(auto it = _online_users.begin(); it != _online_users.end(); ++it) {
      if(*it == user) {
        _online_users.erase(it);
        std::cout << "【用户下线】: " << user.PrintDebug()
                          << " 当前在线人数: " << _online_users.size() << std::endl;
        break;
      }
    }
  }


private:  
  bool _isrunning;
  uint16_t _port;
  int _sockfd;
  std::vector<InetAddr> _online_users;
};