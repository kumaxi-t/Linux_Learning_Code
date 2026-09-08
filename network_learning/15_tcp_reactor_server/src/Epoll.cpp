#include "Epoll.hpp"
#include "Connection.hpp"

Epoll::Epoll(int size) {
  _epoll_fd = epoll_create(size);
  if(_epoll_fd < 0) {
    std::cerr << "epoll_create error" << std::endl;
  }
}

Epoll::~Epoll(){
  if(_epoll_fd >= 0) {
    close(_epoll_fd);
  }
}

bool Epoll::Add(int fd, uint32_t events, Connection* conn) const {
  struct epoll_event ev;

  
  ev.data.ptr = conn;
  ev.events = events;

  if(epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
    std::cerr << "epoll_ctl ADD error" << std::endl;
    return false;
  }
  return true;
}

bool Epoll::Mod(int fd, uint32_t events, Connection* conn) const {
  struct epoll_event ev;
  ev.data.ptr = conn;
  ev.events = events;
  if(epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, fd, &ev) < 0) {
    std::cerr << "epoll_ctl MOD error" << std::endl;
    return false;
  }
  return true;
}

bool Epoll::Del(int fd) const {
  if(epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, nullptr) < 0) {
    std::cerr << "epoll_ctl DEL error" << std::endl;
    return false;
  }
  return true;
}

int Epoll::Wait(struct epoll_event* revents, int maxevents, int timeout) const {
  return epoll_wait(_epoll_fd, revents, maxevents, timeout);
}
