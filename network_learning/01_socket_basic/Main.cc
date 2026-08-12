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

  struct sockaddr_in local;
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
    struct sockaddr_in peer;
    socklen_t len = sizeof(peer);
    ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&peer, &len);


    if(n > 0) {
      buffer[n] = '\0';

      uint16_t client_port = ntohs(peer.sin_port);
      char *client_ip = inet_ntoa(peer.sin_addr);

      std::cout << "收到来自 [" << client_ip << ":" << client_port << "] 的消息: " << buffer << std::endl;

      std::string echo_string = "server echo# ";
      echo_string += buffer;


      ssize_t s = sendto(sockfd, echo_string.c_str(), echo_string.size(), 0, (struct sockaddr *)&peer, len);

      if(s < 0) {
        std::cerr << "sendto error: " << strerror(errno) << std::endl;
      }



    }
  }


  close(sockfd);


  return 0;
}