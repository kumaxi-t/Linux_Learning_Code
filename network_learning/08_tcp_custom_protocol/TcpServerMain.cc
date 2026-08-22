#include <iostream>
#include <memory>
#include "TcpServer.hpp"

// 使用方法: ./tcp_server 8080
int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
    return 1;
  }

  uint16_t port = std::stoi(argv[1]);
  std::unique_ptr<TcpServer> svr = std::make_unique<TcpServer>(port);
  
  svr->Init();
  std::cout << "TcpServer running on port: " << port << "..." << std::endl;
  svr->Loop();

  return 0;
}