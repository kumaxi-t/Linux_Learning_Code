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

bool ParseMessage(std::string& inbuffer, std::string* out_msg) {

  auto pos = inbuffer.find(LineBreakSep);
  if(pos == std::string::npos) return false;

  *out_msg = inbuffer.substr(0, pos);

  inbuffer.erase(0, pos + 1);

  return true;
}

