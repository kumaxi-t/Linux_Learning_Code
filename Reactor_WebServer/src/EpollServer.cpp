#include "../include/EpollServer.hpp"


EpollServer::EpollServer(uint16_t port, BusinessHandler business_cb, int max_events) 
: _port(port), 
  _business_cb(business_cb),
  _max_events(max_events){
    _epoll_ptr = std::make_unique<Epoll>();
    _revents = std::make_unique<struct epoll_event[]>(_max_events);
}

void EpollServer::Init() {
  _listen_socket.CreateSockfd();
  _listen_socket.Bind(_port);
  _listen_socket.Listen();

  int listen_fd = _listen_socket.GetSockfd();
  SetNonBlock(listen_fd);

  Connection* listen_conn = new Connection(listen_fd);

  listen_conn->RegisterCallBack(
    std::bind(&EpollServer::AcceptHandler, this, std::placeholders::_1),
    nullptr,
    nullptr
  );

  _epoll_ptr->Add(listen_fd, EPOLLIN | EPOLLET, listen_conn);

}

void EpollServer::Start() {

  while(true) {
    int nready = _epoll_ptr->Wait(_revents.get(), _max_events, 3000);

    if(nready < 0) {
      if(errno == EINTR) continue;
      std::cerr << "epoll_ctl Wait error" << std::endl;
      return ;
    }else if(nready == 0) {
      continue;
    }else {
      for(int i = 0; i < nready; i++) {
        Connection* conn = static_cast<Connection*> (_revents[i].data.ptr);
        uint32_t cur_events = _revents[i].events;

        if(cur_events & (EPOLLERR | EPOLLHUP)) {
          if(conn->_except_cb) {
            conn->_except_cb(conn);
          }
          continue;
        }
        if((cur_events & EPOLLIN) && conn->_read_cb) {
          conn->_read_cb(conn);
        } 
        if((cur_events & EPOLLOUT) && conn->_write_cb) {
          conn->_write_cb(conn);
        } 

      }
    }
  }
}

void EpollServer::AcceptHandler(Connection* conn) {
  while(true) {
    int client_fd = accept(conn->_sockfd, nullptr, nullptr);
    if(client_fd < 0) {
      if(errno == EINTR) continue;
      if(errno == EAGAIN || errno == EWOULDBLOCK) break;
      std::cerr << "accept error" << std::endl;
      break;
    }else {
      SetNonBlock(client_fd);
      Connection* client_conn = new Connection(client_fd);

      client_conn->RegisterCallBack(
        std::bind(&EpollServer::RecvHandler, this, std::placeholders::_1),
        std::bind(&EpollServer::SendHandler, this, std::placeholders::_1),
        std::bind(&EpollServer::ExceptHandler, this, std::placeholders::_1)
      );
      _epoll_ptr->Add(client_fd, EPOLLIN | EPOLLET, client_conn);
      
    }
  }
}


void EpollServer::RecvHandler(Connection* conn) {
  char buf[4096];
  while(true) {
    ssize_t n = read(conn->_sockfd, buf, sizeof(buf) - 1);
    if(n < 0) {
      if(errno == EINTR) continue;
      if(errno == EAGAIN || errno == EWOULDBLOCK) break;
      ExceptHandler(conn);
      return ;
    }else if (n == 0) {
      ExceptHandler(conn);
      return ;
    }else {
      buf[n] = 0;
      conn->_inbuffer.append(buf, n);
    }
  }

  auto pos = conn->_inbuffer.find("\r\n\r\n");
  if(pos == std::string::npos) {
    return ;
  }
  // 解析 Content-Length，判断是否有请求体（Body）
  size_t header_len = pos + 4;
  size_t content_length = 0;

  std::string header = conn->_inbuffer.substr(0, header_len);
  std::string cl_key = "Content-Length: ";
  auto cl_pos = header.find(cl_key);
  if (cl_pos != std::string::npos) {
      auto end_pos = header.find("\r\n", cl_pos);
      if (end_pos != std::string::npos) {
          std::string cl_str = header.substr(cl_pos + cl_key.size(), end_pos - (cl_pos + cl_key.size()));
          content_length = std::stoul(cl_str);
      }
  }

  // 检查【头部 + 正文】是否全部接收齐备
  size_t total_req_len = header_len + content_length;
  if (conn->_inbuffer.size() < total_req_len) {
      // 说明正文（文件数据）还没收全，继续留在 epoll 等下一次网络数据到来
      return;
  }


  std::string req = conn->_inbuffer.substr(0, total_req_len);
  conn->_inbuffer.erase(0, total_req_len);

  int client_fd = conn->_sockfd;
  _epoll_ptr->Del(client_fd);

  ThreadPoolModule::ThreadPool<>::GetInstance()->Enqueue([client_fd, req](){
    AsyncProcessHttpRequest(client_fd, req);
  });

  delete conn;
  return ;
}


void EpollServer::SendHandler(Connection* conn) {

  while(!conn->_outbuffer.empty()) {
    ssize_t n = write(conn->_sockfd, conn->_outbuffer.c_str(), conn->_outbuffer.size());
    if(n <= 0) {
      if(errno == EINTR) continue;
      if(errno == EAGAIN || errno == EWOULDBLOCK) break;
      ExceptHandler(conn);
      return ;
    }else {
      conn->_outbuffer.erase(0, n);
    }
  }
  if(!conn->_outbuffer.empty()) {
    // 还有数据没发完但是内核写缓冲满了
    _epoll_ptr->Mod(conn->_sockfd, EPOLLIN | EPOLLOUT | EPOLLET, conn);
  }else {
    // 发完了把对写事件的关心关掉，防止CPU空载
    _epoll_ptr->Mod(conn->_sockfd, EPOLLIN | EPOLLET, conn);
  }


}


void EpollServer::ExceptHandler(Connection* conn) {
  int fd = conn->_sockfd;
  _epoll_ptr->Del(fd);
  close(fd);
  delete conn;
  std::cout << "该连接已释放" << std::endl;
}


EpollServer::~EpollServer() {

}

