#pragma once 
#include "Comm.hpp"

struct Connection;

using CallBack = std::function<void(Connection*)>;

struct Connection {

int _sockfd;
std::string _inbuffer;
std::string _outbuffer;

CallBack _read_cb;
CallBack _write_cb;
CallBack _except_cb;


Connection(int sockfd);

void RegisterCallBack(CallBack r, CallBack w, CallBack e);

~Connection();

};