#pragma once
#include "Comm.hpp"
#include "Connection.hpp"


class Epoll {
private:
  int _epfd;


public:
  Epoll(int size = 128);
  
  bool Add(int fd, uint32_t events, Connection* conn);

  bool Mod(int fd, uint32_t events, Connection* conn);

  bool Del(int fd);

  int Wait(struct epoll_event* events, int maxevents, int timeout);


  ~Epoll();

};
