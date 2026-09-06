#include "../include/Comm.hpp"

void SetNonBlock(int fd) {
  int fl = fcntl(fd, F_GETFL);

  if(fl < 0) {
    std::cerr << "fcntl error" << std::endl;
    return ;
  }

  if(fcntl(fd, F_SETFL, fl | O_NONBLOCK) < 0) {
    std::cerr << "fcntl error" << std::endl;
  }
}