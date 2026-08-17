#include "TcpServer.hpp"
#include <memory>

void Usage(const std::string &proc) {
  std::cout << "Usage:\n\t" << proc << " local_port\n\n";
}


int main(int argc, char *argv[]) {
  if(argc != 2) {
    Usage(argv[0]);
    return Usage_Err;
  }

  uint16_t port = std::stoi(argv[1]);
  

  std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(port);

  tsvr->Init();
  tsvr->Start();


  return 0;
}