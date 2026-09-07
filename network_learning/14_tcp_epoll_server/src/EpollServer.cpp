#include "../include/EpollServer.hpp"


EpollServer::EpollServer(uint16_t port, int max_events) 
  :_port(port),
  _listen_fd(-1),
  _max_events(max_events),
  _epoll_ptr(nullptr),
  _revents_ptr(nullptr){
}

EpollServer::~EpollServer() {
  if(_listen_fd >= 0) {
    close(_listen_fd);
  }
}

void EpollServer::Init() {
  _listen_fd = CreateListenSocket(_port);
  if(_listen_fd < 0) {
    std::cerr << "create listen socket error, port: " << _port << std::endl;
    exit(1);
  }
  
  SetNonBlock(_listen_fd);

  _epoll_ptr = std::make_unique<Epoll>(128);
  _revents_ptr = std::make_unique<struct epoll_event[]>(_max_events);

  if(!_epoll_ptr->Add(_listen_fd, EPOLLIN | EPOLLET)) {
    std::cerr << "挂载listen_fd到epoll失败" << std::endl;
    exit(1);
  }

  std::cout << "挂载listen_fd到epoll成功， 监听端口port：" << _port << std::endl;

}

void EpollServer::Start() {
  std::cout << "EpollServer 启动成功，主事件循环开始" << std::endl;
  while(true) {
    int nready = _epoll_ptr->Wait(_revents_ptr.get(), _max_events, 3000);
    if(nready < 0) {
      if(errno == EINTR) continue;
      std::cerr << "epoll_wait error" << std::endl;
      break;
    }else if(nready == 0){
      std::cout << "3s内无事件，服务器运行正常" << std::endl;
      continue;
    }

    for(int i = 0; i < nready; i++) {
      int cur_fd = _revents_ptr[i].data.fd;
      uint32_t cur_events = _revents_ptr[i].events;

      if(cur_fd == _listen_fd && (cur_events & EPOLLIN)) {
        AcceptHandler();
      }else if(cur_events & EPOLLIN){
        RecvHandler(cur_fd);
      }

    }
  }
}


void EpollServer::AcceptHandler() {
  while(true) {

    int client_fd = accept(_listen_fd, nullptr, nullptr);
    if(client_fd < 0) {
      if(errno == EAGAIN || errno == EWOULDBLOCK) return ;
      if(errno == EINTR) continue;
      std::cerr << "accept error" << std::endl;
      return ;
    }

    SetNonBlock(client_fd);

    if(!_epoll_ptr->Add(client_fd, EPOLLIN | EPOLLET)) {
      std::cerr << "epoll_ctl ADD error" << std::endl;
      close(client_fd);
      continue;
    }

    std::cout << "新用户: "<< client_fd << " 挂载成功" << std::endl;
  }
}


void EpollServer::RecvHandler(int client_fd) {
  char buf[8];

  std::string inbuffer;
  while(true) {
    ssize_t s = read(client_fd, buf, sizeof(buf) - 1);

    if(s > 0) {
      buf[s] = 0;
      inbuffer += buf;
      // buf[s] = 0;
      // std::cout << "收到客户端：" << client_fd << " 发来的消息：" << buf << std::endl;
      // std::string echo_msg = "Server echo: ";
      // echo_msg += buf;
      // int n = write(client_fd, echo_msg.c_str(), echo_msg.size());
      // (void)n; 
    }else if(s == 0) {
      std::cout << "客户端 " << client_fd << " 主动断开连接" << std::endl;
      _epoll_ptr->Del(client_fd);
      close(client_fd);
      return ;
    }else {
      if(errno == EAGAIN || errno == EWOULDBLOCK) break;
      if(errno == EINTR) continue;
      std::cerr << "read error" << std::endl;
      _epoll_ptr->Del(client_fd);
      close(client_fd);
      return ;
    }
  }
  if(!inbuffer.empty()) {
    std::cout << "ET 批量读取完毕收到客户端 " << client_fd << " 数据: " << inbuffer;
    std::string echo_msg = "Server echo: " + inbuffer;

    int n = write(client_fd, echo_msg.c_str(), echo_msg.size());
    (void)n;
  }

}








