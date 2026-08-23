#include "Comm.hpp"
#include "Socket.hpp"
#include "InetAddr.hpp"
#include "Log.hpp"

using namespace LogModule;
using namespace Net_Work;

using ioservice_t = std::function<void(std::shared_ptr<Socket> &sock, InetAddr &client)>;

class TcpServer {
public:
  TcpServer(uint16_t port) :
    _port(port),
    _listensockptr(std::make_unique<TcpSocket>()), 
    _isrunning(false) {
    _listensockptr->BuildListenSocketMethod(_port);
  }

  void Start(ioservice_t callback) {
    _isrunning = true;
    while(_isrunning) {
      std::string clientip;
      uint16_t clientport = 0;

      Socket *raw_sock = _listensockptr->AcceptConnection(&clientip, &clientport);
      if(!raw_sock) continue;
      InetAddr client(clientip, clientport);

      std::shared_ptr<Socket> sock(raw_sock);

      pid_t id = fork();
      if(id == 0) {
        _listensockptr->CloseSocket();

        if(fork() > 0) exit(0);
        callback(sock, client);
        sock->CloseSocket();
        exit(0);
      }else if(id > 0) {
        sock->CloseSocket();
        waitpid(id, nullptr, 0);
      }else {
        LOG(LogLevel::FATAL) << "fork error"; 
        exit(FORK_ERR);
      }
      
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