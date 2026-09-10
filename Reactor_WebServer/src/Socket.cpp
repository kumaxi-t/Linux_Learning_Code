#include "../include/Socket.hpp"

Socket::Socket(int sockfd) : _sockfd(sockfd) {

}

Socket::~Socket() {
  close(_sockfd);
}


void Socket::CreateSockfd() {
  _sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(_sockfd < 0) {
    std::cerr << "socket error" << std::endl;
    exit(1);
  }
  
  // TCP 四次挥手的物理规律：
  // 谁主动发起关闭连接（比如你按 Ctrl+C 强杀服务器，服务器作为主动关闭方发送了第一个 FIN 包）
  // 谁的这个连接在最后一步就会陷入 TIME_WAIT 状态
  // 这个状态默认在 Linux 内核里会持续 2MSL（Maximum Segment Lifetime，通常是 60 秒到 2 分钟）
  // 在这段时间内，该端口在内核眼里依然被老连接霸占着。
  // 如果服务器刚挂掉，你想立刻重启它（执行 bind 相同端口），内核会冷酷地拒绝你
  // 这端口还在走 TIME_WAIT 流程呢，不能重复绑定直接抛出 Address already in use
  // setsockopt 设置 SO_REUSEADDR 的作用
  // 大白话讲就是给内核递了一块“免死金牌”——“如果这个端口正处于 TIME_WAIT 状态，请允许我立即强行复用它绑定启动
  int opt = 1;
  setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

}

int Socket::GetSockfd() const {
  return _sockfd;
}

void Socket::Bind(uint16_t port) {
  struct sockaddr_in local;

  memset(&local, 0, sizeof(local));
  local.sin_addr.s_addr = INADDR_ANY;
  local.sin_family = AF_INET;
  local.sin_port = htons(port);

  if(bind(_sockfd, (struct sockaddr *)& local, sizeof(local)) < 0) {
    std::cerr << "bind error" << std::endl;
    exit(2);
  }

}

void Socket::Listen(int backlog) {
  if(listen(_sockfd, backlog) < 0) {
    std::cerr << "listen error" << std::endl;
    exit(3);
  }
}
