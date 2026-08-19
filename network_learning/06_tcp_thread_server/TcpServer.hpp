#include <iostream>
#include "Comm.hpp"
#include "InetAddr.hpp"
#include "nocopy.hpp"

class TcpServer;

class ThreadData {
public:
  ThreadData(int sockfd, const struct sockaddr_in &addr, TcpServer *server_ptr)
  : _sockfd(sockfd), _addr(addr), _server_ptr(server_ptr){

  }

  int Sockfd() const {return _sockfd; };
  InetAddr Addr() const {return _addr; };
  TcpServer *GetServer() const {return _server_ptr; };

private:
  int _sockfd;
  InetAddr _addr;
  TcpServer *_server_ptr;

};

const static int gbacklog = 8;

class TcpServer : public nocopy{
public:

  TcpServer(uint16_t port) 
  : _port(port),
    _isrunning(false),
    _listensockfd(-1) {
     
  }


  void InitServer() {
    _listensockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(_listensockfd == -1) {
      std::cerr << "creat socket error" << std::endl;
      exit(Socket_Err);
    }

    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));

    local.sin_family = AF_INET;
    local.sin_port = htons(_port);
    local.sin_addr.s_addr = INADDR_ANY;

    if(bind(_listensockfd, CONV(&local), sizeof(local)) != 0) {
      std::cerr << "bind error" << std::endl;
      exit(Bind_Err);
    }

    if(listen(_listensockfd, gbacklog) != 0) {
      std::cerr << "listen error" << std::endl;
      exit(Listen_Err);
    }

  }

  void Loop() {
    _isrunning = true;
    while(_isrunning) {
      struct sockaddr_in peer;
      socklen_t len = sizeof(peer);

      int sockfd = accept(_listensockfd, CONV(&peer), &len);
      if(sockfd < 0) {
        std::cerr << "Accept error" << std::endl;
        continue;
      }

      ProcessConnect(sockfd, peer);

    }
    _isrunning = false;
  }


  static void *Thread_routine(void *args) {
    pthread_detach(pthread_self());
    ThreadData *td = static_cast<ThreadData *> (args);

    td->GetServer()->Server(td->Sockfd(), td->Addr());
    
    close(td->Sockfd());
    delete td;

    return nullptr;

  }


  void ProcessConnect(int sockfd, const struct sockaddr_in &peer) {
    pthread_t pid;
    ThreadData *td = new ThreadData(sockfd, peer, this);

    pthread_create(&pid, nullptr, Thread_routine, (void *)td);

    
  }

  void Server(int sockfd, InetAddr addr) {
    char inbuffer[64];
    while(true) {
      int n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);
      if(n > 0) {
        inbuffer[n] = 0;
        std::cout << "[" << addr.PrintDebug() << "]# " << inbuffer;
        std::string echo_string = "echo# " + std::string(inbuffer);
        write(sockfd, echo_string.c_str(), echo_string.size());
      }else if (n == 0) {
        std::cout << "Client exit" << std::endl;
        break;
      }else {
        std::cerr << "read error :" << addr.PrintDebug() << std::endl;
        break;
      }
    }
  }

  ~TcpServer() {
    close(_listensockfd);
  }


private:
  bool _isrunning;
  int _listensockfd;
  uint16_t _port;

};