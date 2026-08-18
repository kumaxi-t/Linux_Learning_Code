#include <iostream>
#include "Comm.hpp"
void Usage(const std::string &proc) {
    std::cout << "Usage:\n\t" << proc << " server_ip server_port\n\n";
}



// 启动方式: ./tcp_client 127.0.0.1 8080
int main(int argc, char *argv[]) {
  if(argc != 3) {
    Usage(argv[0]);
    return Usage_Err;
  }

  std::string server_ip = argv[1];
  uint16_t server_port = std::stoi(argv[2]);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    std::cerr << "socket error" << std::endl;
    return Socket_Err;
  }

  struct sockaddr_in server;
  memset(&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(server_port);

  inet_pton(AF_INET, server_ip.c_str(), &server.sin_addr);

  if(connect(sockfd, CONV(&server), sizeof(server)) != 0) {
    std::cerr << "connect error" << std::endl;
    close(sockfd);
    return 2;
  }

  while(true) {
    std::cout << "Please Enter# " << std::endl;

    std::string message;
    if(!std::getline(std::cin, message)) {
      break;
    }

    // 发送给服务器
    ssize_t s = write(sockfd, message.c_str(), message.size());
    if(s > 0) {
      char buffer[64];
      ssize_t n = read(sockfd, buffer, sizeof(buffer) - 1);
      if(n > 0) {
        buffer[n] = 0;
        std::cout << buffer << std::endl;
      }else {
        break;
      }
    }else {
      break;
    }

  }




  close(sockfd);
  return 0;
}