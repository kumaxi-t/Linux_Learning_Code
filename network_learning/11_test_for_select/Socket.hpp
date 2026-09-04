#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int CreateListenSocket(uint16_t port) {

  int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(listen_fd < 0) {
    std::cerr << "listen error" << std::endl;
    return -1;
  }
  int opt = 1;
  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  struct sockaddr_in local;
  local.sin_family = AF_INET;
  local.sin_addr.s_addr = INADDR_ANY;
  local.sin_port = htons(port);

  if(bind(listen_fd, (struct sockaddr *)&local, sizeof(local)) < 0) {
    std::cerr << "bind error" << std::endl;
    return -1;
  }

  if(listen(listen_fd, 5) < 0) {
    std::cerr << "listen error" << std::endl;
    return -1;
  }

  return listen_fd;
}









