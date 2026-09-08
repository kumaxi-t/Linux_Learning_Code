#pragma once
#include <string>
#include <functional>

struct Connection;
using callback = std::function<void(Connection*)>;


struct Connection {
  int _sock_fd;
  std::string _inbuffer;
  std::string _outbuffer;

  
  callback _read_cb;
  callback _write_cb;
  callback _except_cb;

  explicit Connection(int sock_fd) 
  : _sock_fd(sock_fd),
    _read_cb(nullptr),
    _write_cb(nullptr),
    _except_cb(nullptr) {}


  void RegisterCallBack(callback r, callback w, callback e);
  
};