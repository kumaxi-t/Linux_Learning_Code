#include "EpollServer.hpp"


int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: " << argv[0] << " Port" << std::endl;
    return 1;
  }

  uint16_t port = static_cast<uint16_t>(std::atoi(argv[1]));

  EpollServer server(port);
  server.Init();
  
  server.Start();

  return 0;
}