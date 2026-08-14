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

const static uint16_t defaultport = 8888;
const static int defaultfd = -1;

using func_t = std::function<void(const std::string &req, std::string *resp)>;


class UdpServer {
public:
  UdpServer(func_t func, uint16_t port)
  : _func(func),
    _port(port),
    _sockfd(defaultfd),
    _isrunning(false) {}

  ~UdpServer(){
    if(_sockfd >= 0) {
      close(_sockfd);
    }
  }

  void Init() {
    _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(_sockfd < 0) {
      std::cerr << "socket error: " << strerror(errno) << std::endl;
      exit(1);
    }

    struct sockaddr_in local;
    bzero(&local, sizeof(local));
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_family = AF_INET;
    local.sin_port = htons(_port);

    int n = bind(_sockfd, (struct sockaddr *)&local, sizeof(local));
    if(n < 0) {
      std::cerr << "bind error: " << strerror(errno) << std::endl;
      exit(2); 
    }
    std::cout << "UdpServer 初始化成功，正在监听窗口： " << _port << std::endl;

  }

  void Start() {
    _isrunning = true;
    char buffer[1024];
    while(_isrunning) {
      struct sockaddr_in peer;
      socklen_t len = sizeof(peer);

      // 接受数据
      ssize_t n = recvfrom(_sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);

      if(n > 0) {
        buffer[n] = '\0';
        std::string req = buffer;

        // 去除可能携带的换行符
        if(!req.empty() && req.back() == '\n') req.pop_back();
        if(!req.empty() && req.back() == '\r') req.pop_back();
        
        InetAddr addr(peer);

        std::cout << "收到来自 " << addr.PrintDebug() << " 的请求: " << req << std::endl;

        std::string resp;
        _func(req, &resp);

        // 将计算出的相应数据原路返回
        sendto(_sockfd, resp.c_str(), resp.size(), 0, (struct sockaddr *)&peer, len);
        
      }
    }





  }





  private:
  uint16_t _port;
  int _sockfd;
  bool _isrunning;
  func_t _func;
};



