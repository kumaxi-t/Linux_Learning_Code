#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <string.h>
#include <sys/select.h>
#include <functional>
#include <vector>
#include "Socket.hpp"

std::vector<std::function<void()>> tasks;

void TaskLog() {
  std::cout << "Log Task" << std::endl;
}

void TaskHeartbeat() {
  std::cout << "Heartbeat Task" << std::endl;
}

void RunTasks() {
  for(const auto& task : tasks) {
    task();
  }
}

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

int main(int argc, char *argv[]) {
  if(argc != 2) {
    std::cerr << "Usage: " << argv[0] << " port" << std::endl;
    return 0;
  }
  int listen_fd = CreateListenSocket(std::atoi(argv[1]));

  tasks.push_back(TaskLog);
  tasks.push_back(TaskHeartbeat);

  // 把标准输入改为非阻塞
  SetNonBlock(0);

  std::vector<int> fds;
  fds.push_back(0);
  fds.push_back(listen_fd);

  while(true) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    int max_fd = 0;
    for(auto &fd : fds) {
      FD_SET(fd, &read_fds);
      max_fd = std::max(max_fd, fd);
    }

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int select_ret = select(max_fd + 1, &read_fds, nullptr, nullptr, &timeout);

    if(select_ret < 0) {
      if(errno == EINTR) continue;

      perror("select error");
      break;
    }else if(select_ret == 0) {
      std::cout << "No data, executing background tasks... " << std::endl;
      RunTasks();
      continue;
    }

    for(size_t i = 0; i < fds.size(); i++) {
      int cur_fd = fds[i];
      if(!FD_ISSET(cur_fd, &read_fds)) continue;
      if(cur_fd == listen_fd) {
        int clientfd = accept(listen_fd, nullptr, nullptr);
        if(clientfd > 0) {
          SetNonBlock(clientfd);
          fds.push_back(clientfd);
          std::cout << "New Client Connect: " << clientfd << std::endl;
        }
      }else {
        char client_buf[1024];
        ssize_t s = read(cur_fd, client_buf, sizeof(client_buf) - 1);
        if(s > 0) {
          client_buf[s] = 0;
          std::cout << "Client " << cur_fd << " :" << client_buf << std::endl;
        }else if(s == 0) {
          std::cout << "Client quit" << std::endl;
          close(cur_fd);
          fds.erase(fds.begin() + i);
          i--;
        }else {
          if(errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
            std::cerr << "Client error" << std::endl;
            close(cur_fd);
            fds.erase(fds.begin() + i);
            i--;
          }
        }

      }

    }
    
    




  }

  return 0;
}

