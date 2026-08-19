#include <iostream>
#include "TcpServer.hpp"
void Usage(const std::string &proc) {
  std::cout << "Usage:\n\t " << proc << " local_port" << std::endl;
}


int main(int argc, char *argv[]) {
  if(argc != 2) {
    Usage(argv[0]);
    return Usage_Err;
  }

  uint16_t port = std::stoi(argv[1]);

  std::unique_ptr<TcpServer> svr = std::make_unique<TcpServer>(port);

  svr->InitServer();
  svr->Loop();



  return 0;
}