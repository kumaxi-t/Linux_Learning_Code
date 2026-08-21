#pragma once
#include "Comm.hpp"
#include "InetAddr.hpp"
#include "Command.hpp"

using command_server_t = std::function<std::string(const std::string &)>;
class Task {
public:
  Task() : _sockfd(-1){

  }
  Task(int sockfd, const InetAddr &addr, command_server_t server)
  : _sockfd(sockfd),
    _addr(addr),
    _server(server) {

  }

  void operator()() {
    char inbuffer[64];
    while(true) {
      int n = read(_sockfd, inbuffer, sizeof(inbuffer) - 1);
      if(n > 0) {
        inbuffer[n] = 0;
        std::string cmd = inbuffer;

        while(!cmd.empty() && (cmd.back() == '\n' || cmd.back() == '\r')) cmd.pop_back();

        if(cmd.empty()) continue;

        std::string resp = _server(cmd);
        write(_sockfd, resp.c_str(), resp.size());

      }else if (n == 0) {
        std::cout << "Client exit" << std::endl;
        break;
      }else {
        std::cerr << "read error" << std::endl;
        break;
      }


    }

    close(_sockfd);
  }


  ~Task() {

  }

private:
  InetAddr _addr;
  command_server_t _server;
  int _sockfd;

};