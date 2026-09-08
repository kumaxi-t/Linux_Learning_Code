#include "EpollServer.hpp"
// 业务1：简单的回声增强版
std::string EchoService(const std::string& req) {
    return "[Echo from Reactor] " + req;
}

// 业务2：将客户端发来的英文字符全部转为大写
std::string ToUpperService(const std::string& req) {
    std::string resp = req;
    for (char& c : resp) {
        c = std::toupper(c);
    }
    return "[Uppercase Result] " + resp;
}

int main(int argc, char* argv[]) {
  if(argc != 2) {
    std::cout << "Usage: " << argv[0] << " Port" << std::endl;
    return 1;
  }

  uint16_t port = static_cast<uint16_t>(std::atoi(argv[1]));

  EpollServer server(port, ToUpperService);
  server.Init();
  
  server.Start();

  return 0;
}