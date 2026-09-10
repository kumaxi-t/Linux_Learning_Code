#include "../include/Comm.hpp"


bool SetNonBlock(int fd) {
  int flags = fcntl(fd, F_GETFL);
  if(flags < 0) {
    std::cerr << "fcntl error" << std::endl;
    return false;
  }
  if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    std::cerr << "fcntl error" << std::endl;
    return false;
  }
  return true;
}

std::string ReadFile(const std::string& path) {
  std::ifstream file(path, std::ios::binary);

  if(!file.is_open()) {
    return "";
  }
  std::stringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

