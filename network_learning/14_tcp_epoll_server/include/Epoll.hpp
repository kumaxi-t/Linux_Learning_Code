#pragma once
#include "Comm.hpp"
class Epoll {
private:
  int _epoll_fd;

public:
  explicit Epoll(int size = 128);

  ~Epoll();

  Epoll(const Epoll& ) = delete;
  Epoll& operator=(const Epoll& ) = delete;

  bool Add(int fd, uint32_t events) const;

  bool Mod(int fd, uint32_t events) const;

  bool Del(int fd) const;

  int Wait(struct epoll_event* revents, int maxevents, int timeout = 3000) const;

};