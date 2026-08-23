#include "Http.hpp"
#include <memory>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "Usage:\n\t" << argv[0] << " port" << std::endl;
    return 1;
  }
  uint16_t port = std::stoi(argv[1]);

  std::unique_ptr<Http> server = std::make_unique<Http>(port);
  server->Start();

  return 0;
}