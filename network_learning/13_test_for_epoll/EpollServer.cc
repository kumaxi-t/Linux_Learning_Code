#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <cstring>
#include <cerrno>
#include <fcntl.h>

void SetNonBlock(int fd) {
  int fl = fcntl(fd, F_GETFL);
  if(fl < 0) {
    perror("fcntl F_GETFL error");
    return;
  }

  fl |= O_NONBLOCK;

  if(fcntl(fd, F_SETFL, fl) < 0) {
    perror("fcntl error");
    return;
  }
}



int main() {
  SetNonBlock(0);

  int epfd = epoll_create(128);
  if(epfd < 0) {
    std::cerr << "epoll_create error" << std::endl;
    return -1;
  }
  std::cout << "epoll create success, take fd: " << epfd << std::endl;


  struct epoll_event ev;
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = 0;

  if(epoll_ctl(epfd, EPOLL_CTL_ADD, 0, &ev) < 0) {
    std::cerr << "epoll_ctl error" << std::endl;
    close(epfd);
    return 1;
  }

  std::cout << "将键盘输入成功挂载到epoll红黑树上" << std::endl;


  struct epoll_event revent[10];


  while(true) {

    int nready = epoll_wait(epfd, revent, 10, 3000);

    if(nready < 0) {
      if(errno == EINTR) continue;
      std::cerr << "epoll_wait errnr" << std::endl;
      break;
    }else if(nready == 0) {
      std::cout << "3s内无键盘输入，继续监听..." << std::endl;
      continue;
    }

    std::cout << "检测到就绪事件，本次数量：" << nready << std::endl;


    for(int i = 0; i < nready; i++) {
      int cur_fd = revent[i].data.fd;

      if(revent[i].events & EPOLLIN) {
        if(cur_fd == 0) {
          char buf[128];
          char ch;
          while(true) {
            ssize_t s = read(cur_fd, &ch, 1);
            if(s > 0) {
              std::cout << ch;
              // buf[s] = 0;
              // std::cout << "success read, content:" << ch << std::endl;
            }else if(s == 0) {
              std::cout << "收到EOF， 退出程序" << std::endl;
              close(epfd);
              return 0;
            }else {
              if(errno == EAGAIN || errno == EWOULDBLOCK) {
                std::cout << "数据已读取完毕" << std::endl;
                break;
              }
              if(errno == EINTR) continue;
              std::cerr << "read error" << std::endl;
              break;
            }
          }
        }
      }
    }




  }











  close(epfd);


  return 0;
}