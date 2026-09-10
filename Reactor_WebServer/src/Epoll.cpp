#include "../include/Epoll.hpp"


Epoll::Epoll(int size) {
  _epfd = epoll_create(size);
  if(_epfd < 0) {
    std::cerr << "epoll_create error" << std::endl;
    exit(4);
  }
}

bool Epoll::Add(int fd, uint32_t events, Connection* conn) {
  struct epoll_event ev;
  ev.data.ptr = conn;
  ev.events = events;
  if(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) < 0) {
    std::cerr << "epoll_ctl Add error" << std::endl;
    return false;
  }
  return true;
}

bool Epoll::Mod(int fd, uint32_t events, Connection* conn) {
  struct epoll_event ev;
  ev.data.ptr = conn;
  ev.events = events;
  if(epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev) < 0) {
    std::cerr << "epoll_ctl Mod error" << std::endl;
    return false;
  }
  return true;
}

bool Epoll::Del(int fd) {
  if(epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, nullptr) < 0) {
    return false;
  }
  return true;
}

int Epoll::Wait(struct epoll_event* events, int maxevents, int timeout) {
  return epoll_wait(_epfd, events, maxevents, timeout);
}




Epoll::~Epoll() {
  if(_epfd >= 0) {
    close(_epfd);
  }
}