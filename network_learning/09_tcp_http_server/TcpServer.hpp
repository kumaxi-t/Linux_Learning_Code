#include "Comm.hpp"
#include "Socket.hpp"
#include "InetAddr.hpp"
#include "Log.hpp"

using namespace LogModule;
using namespace Net_Work;

using ioservice_t = std::function<void(std::shared_ptr<Socket> &sock, InetAddr &client)>;

class TcpServer;

struct ThreadData {
    std::shared_ptr<Socket> sock;
    InetAddr client;
    ioservice_t callback;
};
class TcpServer {
public:
  TcpServer(uint16_t port) :
    _port(port),
    _listensockptr(std::make_unique<TcpSocket>()), 
    _isrunning(false) {
    _listensockptr->BuildListenSocketMethod(_port);
  }
  static void *Routine(void *args) {
    pthread_detach(pthread_self()); 
    ThreadData *td = static_cast<ThreadData *>(args);
    
    td->callback(td->sock, td->client);
    td->sock->CloseSocket();
    
    delete td; 
    return nullptr;
  }
  
  void Start(ioservice_t callback) {
    _isrunning = true;
    while(_isrunning) {
      std::string clientip;
      uint16_t clientport = 0;

      Socket *raw_sock = _listensockptr->AcceptConnection(&clientip, &clientport);
      if(!raw_sock) continue;

      // 简易多线程
      // 避免多进程bug以及单进程卡死
      ThreadData *td = new ThreadData();
      td->sock = std::shared_ptr<Socket>(raw_sock);
      td->client = InetAddr(clientip, clientport);
      td->callback = callback;

      pthread_t tid;
      pthread_create(&tid, nullptr, Routine, td);

      // InetAddr client(clientip, clientport);

      // std::shared_ptr<Socket> sock(raw_sock);

      // 单进程
      // callback(sock, client);
      // sock->CloseSocket();

      // 多进程 g_sessions会有bug
      // 第二次请求访问 /user 时又 fork() 出了全新的子进程 B
      // 子进程 B 拥有一个全新完全为空的 g_sessions，永远查不到之前的登录凭证
      // pid_t id = fork();
      // if(id == 0) {
      //   _listensockptr->CloseSocket();

      //   if(fork() > 0) exit(0);
      //   callback(sock, client);
      //   sock->CloseSocket();
      //   exit(0);
      // }else if(id > 0) {
      //   sock->CloseSocket();
      //   waitpid(id, nullptr, 0);
      // }else {
      //   LOG(LogLevel::FATAL) << "fork error"; 
      //   exit(FORK_ERR);
      // }
      
    }
    _isrunning = false;

  }
  ~TcpServer() {

  }
private:
  std::unique_ptr<Socket> _listensockptr;
  uint16_t _port;
  bool _isrunning;
};