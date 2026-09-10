#pragma once
#include "Comm.hpp"
#include "Socket.hpp"
#include "Epoll.hpp"
#include "Connection.hpp"

using BusinessHandler = std::function<std::string(const std::string&)>;


class EpollServer {

private:

  uint16_t _port;
  Socket _listen_socket;
  std::unique_ptr<Epoll> _epoll_ptr;
  std::unique_ptr<struct epoll_event[]> _revents;

  int _max_events;
  BusinessHandler _business_cb;



public:

  EpollServer(uint16_t port, BusinessHandler business_cb, int max_events = 64);

  void Init();

  void Start();

  void AcceptHandler(Connection* conn);
  void RecvHandler(Connection* conn);
  void SendHandler(Connection* conn);
  void ExceptHandler(Connection* conn);


  ~EpollServer();
};