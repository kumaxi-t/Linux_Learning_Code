#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <netinet/in.h>
#include <arpa/inet.h>


int main() {
  //       int socket(int domain, int type, int protocol);
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  if(sockfd < 0) {
    std::cerr << "Socket创建失败，错误码： " << errno << ", 原因： " << strerror(errno) << std::endl;
    return 1;
  }

  std::cout << "Socket创建成功， fd: " << sockfd << std::endl;

  struct sockaddr_in local;          // 我（服务端） 的 IP 和端口是什么
  bzero(&local, sizeof(local));

  local.sin_family = AF_INET;
  local.sin_port = htons(8888);
  local.sin_addr.s_addr = INADDR_ANY;

  int n = bind(sockfd, (struct sockaddr *)&local, sizeof(local));
  if(n < 0) {
    std::cerr << "bind error: " <<  strerror(errno) << std::endl;
    close(sockfd);
    return 2; 
  }

  std::cout << "bind success, 8888 fine" << std::endl;

  char buffer[64];
  while(true) {
    struct sockaddr_in peer;        // 对方（客户端） 的 IP 和端口
    socklen_t len = sizeof(peer);
    // 核心收包函数。如果现在没有网络数据进来，程序就会卡在这里不动（阻塞等待）
    ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);


    if(n > 0) {
      buffer[n] = '\0';
      // 把内核帮我们填好的客户端网络端口号，从“大端序”转回我们电脑看得懂的“小端序”（network to host short）
      uint16_t client_port = ntohs(peer.sin_port);
      // 内核填在 peer 里的 IP 是一个 32 位的数字（如 2130706433）
      // 这个函数负责把这个数字转成人类能看懂的点分十进制字符串（如 "127.0.0.1"）
      char *client_ip = inet_ntoa(peer.sin_addr);

      std::cout << "收到来自 [" << client_ip << ":" << client_port << "] 的消息: " << buffer << std::endl;

      std::string echo_string = "server echo# ";
      echo_string += buffer;

      // 发包系统函数
      // 把 buffer 里的内容，通过套接字 sockfd（数字 3），根据刚才 recvfrom 拿到的回邮地址 peer，精准原路寄回给客户端
      ssize_t s = sendto(sockfd, echo_string.c_str(), echo_string.size(), 0, (struct sockaddr *)&peer, len);

      if(s < 0) {
        std::cerr << "sendto error: " << strerror(errno) << std::endl;
      }

    }
  }


  close(sockfd);


  return 0;
}