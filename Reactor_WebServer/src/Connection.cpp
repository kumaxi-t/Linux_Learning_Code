#include "../include/Connection.hpp"
#include "../include/Connection.hpp"

Connection::Connection(int sockfd) 
  : _sockfd(sockfd),
    _read_cb(nullptr),
    _write_cb(nullptr),
    _except_cb(nullptr) {

}


void Connection::RegisterCallBack(CallBack r, CallBack w, CallBack e) {
  _read_cb = r;
  _write_cb = w;
  _except_cb = e;
}



Connection::~Connection() {
  // close(_sockfd);    // Bug
}

