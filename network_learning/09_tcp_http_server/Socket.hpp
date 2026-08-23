#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define Convert(addrptr) ((struct sockaddr *)addrptr)

namespace Net_Work
{
  const static int defaultsockfd = -1;
  const int backlog = 5;

  enum {
    SocketError = 1,
    BindError,
    ListenError
  };


  class Socket {
  public:
    virtual ~Socket() {}

    virtual void CreateSocketOrDie() = 0;
    virtual void BindSocketOrDie(uint16_t port) = 0;
    virtual void ListenSocketOrDie(int backlog) = 0;

    virtual Socket *AcceptConnection(std::string *peerip, uint16_t *peerport) = 0;

    virtual bool ConnectServer(const std::string &serverip, uint16_t serverport) = 0;

    virtual int GetSockFd() = 0;
    virtual void SetSockFd(int sockfd) = 0;
    virtual void CloseSocket() = 0;

    virtual bool Recv(std::string *buffer, int size) = 0;
    virtual void Send(const std::string &send_str) = 0;

  public:
    // 服务端创建并监听
    void BuildListenSocketMethod(uint16_t port, int backlog = 5) {
      CreateSocketOrDie();
      BindSocketOrDie(port);
      ListenSocketOrDie(backlog);
    }

    // 客户端创建并连接
    bool BuildConnectSocketMethod(const std::string &serverip, uint16_t serverport) {
      CreateSocketOrDie();
      return ConnectServer(serverip, serverport);
    }

    // 把原生描述符封装进对象
    void BuildNormalSocketMethod(int sockfd) {
      SetSockFd(sockfd);
    }
  };


  class TcpSocket : public Socket {
  public:
    TcpSocket(int sockfd = defaultsockfd) : _sockfd(sockfd) {}
    ~TcpSocket() {}

    // 创建套接字
    void CreateSocketOrDie() override {
      _sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
      if(_sockfd < 0) {
        std::cerr << "socket error" << std::endl;
        exit(SocketError);
      }
    }

    // 绑定端口
    void BindSocketOrDie(uint16_t port) override {
      struct sockaddr_in local;
      memset(&local, 0, sizeof(local));
      int opt = 1;
      setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
      local.sin_family = AF_INET;
      local.sin_addr.s_addr = INADDR_ANY;
      local.sin_port = htons(port);

      if(::bind(_sockfd, Convert(&local), sizeof(local)) < 0) {
        std::cerr << "bind error" << std::endl;
        exit(BindError);
      }
      
    }

    // 开始监听
    void ListenSocketOrDie(int backlog) override {

      if(::listen(_sockfd, backlog) < 0) {
        std::cerr << "listen error" << std::endl;
        exit(ListenError);
      }
    }

    // 接收客户端连接
    Socket *AcceptConnection(std::string *peerip, uint16_t *peerport) override {

      struct sockaddr_in peer;
      socklen_t len = sizeof(peer);
      int sockfd = ::accept(_sockfd, Convert(&peer), &len);
      if(sockfd < 0) {
        return nullptr;
      }
      if(peerport) *peerport = ntohs(peer.sin_port);
      if(peerip) *peerip = inet_ntoa(peer.sin_addr);
      return new TcpSocket(sockfd);
    }

    // 客户端连接服务器
    bool ConnectServer(const std::string &serverip, uint16_t serverport) override {
      struct sockaddr_in server;
      memset(&server, 0, sizeof(server));

      server.sin_family = AF_INET;
      server.sin_addr.s_addr = inet_addr(serverip.c_str());
      server.sin_port = htons(serverport);
      
      int n = ::connect(_sockfd, Convert(&server), sizeof(server));
      return n == 0;
    }

    int GetSockFd() override { return _sockfd; }
    void SetSockFd(int sockfd) override { _sockfd = sockfd; }

    void CloseSocket() override {
      if (_sockfd > defaultsockfd) { 
        ::close(_sockfd);
        _sockfd = defaultsockfd; }
    }

    // 接收并追加到用户 buffer
    bool Recv(std::string *buffer, int size = 1024) override {
      char inbuffer[size];
      ssize_t n = ::recv(_sockfd, inbuffer, size - 1, 0);
      if (n > 0) { 
        inbuffer[n] = 0; 
        *buffer += inbuffer; 
        return true; 
      }
      else return false;
    }

    // 发送数据
    void Send(const std::string &send_str) override {
      ::send(_sockfd, send_str.c_str(), send_str.size(), 0);
    }

  private:
    int _sockfd;
  };
}
// end Net_Work

