#pragma once
#include "Comm.hpp"


const int defaultfd = -1;

class Socket{
private:
  int _sockfd;


public:

  Socket(int sockfd = defaultfd);

  int GetSockfd() const;

  void CreateSockfd();

  void Bind(uint16_t port);

  void Listen(int backlog = 128);

  ~Socket();

};