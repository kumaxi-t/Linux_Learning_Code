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