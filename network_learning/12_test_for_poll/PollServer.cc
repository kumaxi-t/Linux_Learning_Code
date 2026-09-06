#include <iostream>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <cerrno>
#include <sys/socket.h>
#include <fcntl.h>
#include "Socket.hpp"

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

void RunPollServer(int listen_fd) {

  std::vector<struct pollfd> poll_fds;
  
  struct pollfd listen_pfd;
  listen_pfd.fd = listen_fd;
  listen_pfd.events = POLLIN;
  listen_pfd.revents = 0;

  poll_fds.emplace_back(listen_pfd);

  std::cout << "Poll Server Init Success! " << std::endl;

  while(true) {

    int nready = poll(poll_fds.data(), poll_fds.size(), 3000);

    if(nready < 0) {
      if(errno == EINTR) continue;
      std::cerr << "Poll Error" << std::endl;
      break;
    }else if(nready == 0){
      std::cout << "3s内无具事件" << std::endl;
      continue;
    }

    std::cout << "检查到事件， 当前有" << nready << "个连接已就绪" << std::endl;

    size_t current_size = poll_fds.size();

    for(size_t i = 0; i < current_size; i++) {
      if(poll_fds[i].fd < 0 || !(poll_fds[i].revents & POLLIN)) continue;

      if(poll_fds[i].fd == listen_fd) {

        int client_fd = accept(listen_fd, nullptr, nullptr);
        if(client_fd < 0) {
          if(errno == EAGAIN || errno == EWOULDBLOCK) continue;
          std::cerr << "accept error" << std::endl;
          continue;
        }

        SetNonBlock(client_fd);
        int reused = false;

        for(size_t j = 0; j < poll_fds.size(); j++) {
          if(poll_fds[j].fd == -1) {
            poll_fds[j].fd = client_fd;
            poll_fds[j].events = POLLIN;
            poll_fds[j].revents = 0;
            reused = true;
            std::cout << "复用已有的卡槽位置: " << j << "对应fd:" << poll_fds[j].fd << std::endl;
            break;
          }
        }
        if(!reused) {
          struct pollfd client_pfd;

          client_pfd.fd = client_fd;
          client_pfd.events = POLLIN;
          client_pfd.revents = 0;
          poll_fds.emplace_back(client_pfd);
          std::cout << "卡槽已满，新增卡槽： " << poll_fds[i].fd << std::endl;
        }

        std::cout << "新用户连接成功， fd:" << client_fd << " ,当前总连接数：" << poll_fds.size() << std::endl;


      }else {

        char client_buf[1024];
        ssize_t s = read(poll_fds[i].fd, client_buf, sizeof(client_buf) - 1);

        if(s > 0) {
          client_buf[s] = 0;
          std::cout << "成功读取clinet" << poll_fds[i].fd << "的数据：" << client_buf << std::endl;
        }else if(s == 0) {
          std::cout << "客户端client" << poll_fds[i].fd << "断开连接" << std::endl;
          close(poll_fds[i].fd);
          poll_fds[i].fd = -1;
        }else {
          if(errno == EAGAIN || errno == EWOULDBLOCK) continue;
          std::cerr << "read error" << std::endl;
          close(poll_fds[i].fd);
          poll_fds[i].fd = -1;
        }
      }
    }
  }
}


int main(int argc, char* argv[]) {

  if(argc != 2) {
    std::cout << "Usage:" << argv[0] << " Port" << std::endl;
    return 1;
  }
  int Port = std::atoi(argv[1]);

  int listen_fd = CreateListenSocket(Port);

  RunPollServer(listen_fd);

  // struct pollfd my_card[1];
  // my_card[0].fd = 0;
  // my_card[0].events = POLLIN;

  // std::cout << "Running...." << std::endl;

  // while(true) {

  //   poll(my_card, 1, -1);
    
  //   if(my_card[0].revents & POLLIN) {
  //     char buf[128];
  //     char ch;
  //     ssize_t s = read(0, &ch, 1);
  //     if(s > 0) {
  //       // buf[s] = 0;
  //       std::cout << "Say: " << ch << std::endl;

  //     }
  //   }


  // }


  return 0;
}