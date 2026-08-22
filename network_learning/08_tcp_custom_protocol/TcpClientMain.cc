#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Protocol.hpp"

// ./tcp_client 127.0.0.1 8080
int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <server_ip> <server_port>" << std::endl;
    return 1;
  }

  std::string server_ip = argv[1];
  uint16_t server_port = std::stoi(argv[2]);

  int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in client;
  memset(&client, 0, sizeof(client));
  client.sin_family = AF_INET;
  inet_pton(AF_INET, server_ip.c_str(), &client.sin_addr);
  client.sin_port = htons(server_port);

  if(::connect(sockfd, (struct sockaddr *)&client, sizeof(client)) < 0) {
    std::cerr << "connect error" << std::endl;
    return 2;
  }


  std::string inbuffer;
  while (true) {

    std::cout << "请输入两个整数和一个运算符 (例如: 10 20 +): ";
    int x, y;
    char op;
    std::cin >> x >> y >> op;

    // 构建 Request 对象并进行序列化 -> Encode打包
    Protocol::Request req(x, y, op);
    std::string req_json;
    // 序列化
    req.Serialize(&req_json);
    // 打包
    std::string send_pkg = Protocol::Encode(req_json);

    // 发送给服务端: 
    ::send(sockfd, send_pkg.c_str(), send_pkg.size(), 0);

    // 接收服务端的响应并打印
    while(true) {
      char buf[128];
      ssize_t n = ::recv(sockfd, buf, sizeof(buf) - 1, 0);
      if(n > 0) {
        buf[n] = 0;
        inbuffer += buf;

        std::string package;
        // 从inbuffer里把package取出来
        if(Protocol::Decode(inbuffer, &package)){
          Protocol::Response resp;
          // 反序列化获取计算结果
          resp.Deserialize(package);
          std::cout << "Result: " << resp.GetResult() << std::endl;
          std::cout << "Code: " << resp.GetCode() << std::endl;
          break;
        }
      }else {
        std::cerr << "Server closed or recv error" << std::endl;
        ::close(sockfd);
        return 0;
      }

    }

  }

  ::close(sockfd);
  return 0;
}