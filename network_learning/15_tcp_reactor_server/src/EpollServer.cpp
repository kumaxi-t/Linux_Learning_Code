#include "../include/EpollServer.hpp"
#include "Connection.hpp"

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


void EpollServer::ExceptHandler(Connection* conn) {
  int client_fd = conn->_sock_fd;
  _epoll_ptr->Del(client_fd);
  close(client_fd);
  delete conn;

  std::cout << "[fd: " << client_fd << "] 连接已统一安全释放并注销" << std::endl;
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

  Connection* conn = new Connection(_listen_fd);

  conn->RegisterCallBack(std::bind(&EpollServer::AcceptHandler, this, std::placeholders::_1), nullptr, nullptr);


  if(!_epoll_ptr->Add(_listen_fd, EPOLLIN | EPOLLET, conn)) {
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
      Connection* conn = static_cast<Connection*> (_revents_ptr[i].data.ptr);
      uint32_t cur_events = _revents_ptr[i].events;
      int client_fd = conn->_sock_fd;

      if(cur_events & (EPOLLERR | EPOLLHUP) && conn->_except_cb) {
        conn->_except_cb(conn);
        continue;
      }

      if((cur_events & EPOLLIN) && conn->_read_cb){
        conn->_read_cb(conn);
      }
      if ((cur_events & EPOLLOUT) && conn->_write_cb) {
        conn->_write_cb(conn);
      }
    }
  }
}


void EpollServer::AcceptHandler(Connection* conn){
  (void)conn;
  
  while(true) {

    int client_fd = accept(_listen_fd, nullptr, nullptr);
    if(client_fd < 0) {
      if(errno == EAGAIN || errno == EWOULDBLOCK) return ;
      if(errno == EINTR) continue;
      std::cerr << "accept error" << std::endl;
      return ;
    }

    SetNonBlock(client_fd);
    Connection* conn = new Connection(client_fd);

    conn->RegisterCallBack(
      std::bind(&EpollServer::RecvHandler, this, std::placeholders::_1),
      std::bind(&EpollServer::SendHandler, this, std::placeholders::_1),
      std::bind(&EpollServer::ExceptHandler, this, std::placeholders::_1)
    );

    if(!_epoll_ptr->Add(client_fd, EPOLLIN | EPOLLET, conn)) {
      std::cerr << "epoll_ctl ADD error" << std::endl;
      ExceptHandler(conn);
      continue;
    }

    std::cout << "新用户: "<< client_fd << " 挂载成功" << std::endl;
  }
}


void EpollServer::RecvHandler(Connection* conn) {
  char buf[8];

  std::string inbuffer;
  int client_fd = conn->_sock_fd;
  while(true) {
    ssize_t s = read(client_fd, buf, sizeof(buf) - 1);

    if(s > 0) {
      buf[s] = 0;
      conn->_inbuffer += buf;
      
    }else if(s == 0) {
      std::cout << "客户端 " << client_fd << " 主动断开连接" << std::endl;
      ExceptHandler(conn);
      return ;
    }else {
      if(errno == EAGAIN || errno == EWOULDBLOCK) break;
      if(errno == EINTR) continue;
      std::cerr << "read error" << std::endl;
      ExceptHandler(conn);
      return ;
    }
  }

  std::string msg;

  while(ParseMessage(conn->_inbuffer, &msg)) {

  std::cout << ">>> 成功提取完整业务指令: [" << msg << "]" << std::endl;

  // 模拟业务响应：拼装好响应并带上定界符
  std::string response = "[Server echo] " + msg + "\n";
  conn->_outbuffer += response;

  // 调用发送逻辑尝试直接发送
  SendHandler(conn);
  }

  std::cout << "[fd: " << conn->_sock_fd << "] 当前 inbuffer 剩余未完结字节: " 
              << conn->_inbuffer.size() << std::endl;


              
  // std::cout << "[fd: " << conn->_sock_fd << "]当前 inbuffer 累计内容 (" 
  //             << conn->_inbuffer.size() << "字节): [" 
  //             << conn->_inbuffer << "]" << std::endl;
  // if(!inbuffer.empty()) {
  //   std::cout << "ET 批量读取完毕收到客户端 " << client_fd << " 数据: " << inbuffer;
  //   std::string echo_msg = "Server echo: " + inbuffer;

  //   int n = write(client_fd, echo_msg.c_str(), echo_msg.size());
  //   (void)n;
  // }

}


void EpollServer::SendHandler(Connection* conn) {

  int client_fd = conn->_sock_fd;
  while(!conn->_outbuffer.empty()) {

    ssize_t s = write(client_fd, conn->_outbuffer.c_str(), conn->_outbuffer.size());

    if(s > 0) {
      conn->_outbuffer.erase(0, s);
    }else{
      if(errno == EAGAIN || errno == EWOULDBLOCK) break;
      if(errno == EINTR) continue;

      std::cerr << "write error" << std::endl;
      ExceptHandler(conn);
      return ;
    }
  }

  if(!conn->_outbuffer.empty()) {
    _epoll_ptr->Mod(client_fd, EPOLLIN | EPOLLOUT | EPOLLET, conn);
    std::cout << "[fd: " << client_fd << "] 发送缓冲区满，已挂载 EPOLLOUT 托管等待" << std::endl;
  }else {
    _epoll_ptr->Mod(client_fd, EPOLLIN | EPOLLET, conn);
    std::cout << "[fd: " << client_fd << "] 数据已全部排空，关闭 EPOLLOUT" << std::endl;
  }


}





