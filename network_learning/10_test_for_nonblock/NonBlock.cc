#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <string.h>

void SetNonBlock(int fd) {
  int fl = fcntl(fd, F_GETFL);
  if(fl < 0) {
    perror("fcntl F_GETFL error");
    return ;
  }
  fl |= O_NONBLOCK;
  
  if(fcntl(fd, F_SETFL, fl) < 0) {
    perror("fcntl error");
    return ;
  }

}

int main() {
  // 把标准输入改为非阻塞
  SetNonBlock(0);

  char buf[128];
  while(true) {
    memset(buf, 0, sizeof(buf));  
    ssize_t read_size = read(0, buf, sizeof(buf) - 1);
    if(read_size < 0) {
      if(errno == EAGAIN || errno == EWOULDBLOCK) {
        std::cout << "NONE of data..." << std::endl;
        sleep(1);
        continue;
      } else if(errno == EINTR) {
        continue;
      }else {
        perror("read error");
        break;
      }
    }else if(read_size == 0) {
      break;
    }else {
      buf[read_size] = 0;
      std::cout << "read success, content: " << buf << std::endl;
    }
  }

  return 0;
}

