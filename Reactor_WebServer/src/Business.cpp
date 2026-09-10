#include "../include/Business.hpp"




// 定义一个业务回调函数将输入转成大写
std::string ToUpperService(const std::string& req) {
    std::string resp = req;
    for (char& c : resp) {
        c = std::toupper(c);
    }
    return "[Server Echo]: " + resp;
}


std::string SimpleHttpHandler(const std::string& req) {
    // 准备一个网页内容
    std::string html = "<html><head><meta charset='utf-8'></head><body><h1>Hello! 恭喜你，手写的 Reactor Web 服务器跑通了！</h1></body></html>";

    // 按照 HTTP 规则拼装响应
    std::string resp = "HTTP/1.1 200 OK\r\n";
    resp += "Content-Type: text/html; charset=utf-8\r\n";
    resp += "Content-Length: " + std::to_string(html.size()) + "\r\n";
    resp += "Connection: close\r\n";
    resp += "\r\n"; 
    resp += html;   

    return resp;
}


void AsyncProcessHttpRequest(int fd, std::string req) {
  Http httphandler;
  std::string resp = httphandler.HttpHandler(req);

  if(!resp.empty()) {
    ssize_t total_sent = 0;
    size_t to_send = resp.size();
    const char* buf = resp.c_str();

    while(total_sent < to_send) {
      ssize_t s = write(fd, buf + total_sent, to_send - total_sent);
      if(s > 0) {
        total_sent += s;
      }else {
        if(errno == EINTR) continue;
        break;
      }
    }
  }
  close(fd);
}








