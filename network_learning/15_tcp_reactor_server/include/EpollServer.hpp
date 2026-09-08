#pragma once
#include "Comm.hpp"
#include "Epoll.hpp"
#include "Socket.hpp"
#include "Connection.hpp"
class EpollServer {
private:  
  uint16_t _port;
  int _listen_fd;
  int _max_events;
  std::unique_ptr<Epoll> _epoll_ptr;
  std::unique_ptr<struct epoll_event[]> _revents_ptr;


public:
  explicit EpollServer(uint16_t port, int max_events = 1024);

  EpollServer(const EpollServer& ) = delete;
  EpollServer& operator=(const EpollServer& ) = delete;

  void Init();

  void Start();

  ~EpollServer();

private:
  void AcceptHandler(Connection* conn);

  void RecvHandler(Connection* conn);

  void SendHandler(Connection* conn);

  void ExceptHandler(Connection* conn);
};
