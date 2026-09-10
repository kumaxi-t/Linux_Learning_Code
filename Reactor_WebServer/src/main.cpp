#include <iostream>
#include <string>
#include <algorithm>
#include "../include/EpollServer.hpp"

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


std::string DynamicHttpHandler(const std::string& req) {
  auto pos = req.find("\r\n");
  if(pos == std::string::npos) return "";
  std::string req_line = req.substr(0, pos);

  std::string method, url, version;
  std::stringstream ss(req_line);
  ss >> method >> url >> version;

  std::string path;
  if(url == "/") {
    path = "wwwroot/index.html";
  }else {
    path = "wwwroot" + url;
  }

  std::string content = ReadFile(path);
  std::string status;
  std::string mime_type;

  if(!content.empty()) {
    status = "HTTP/1.1 200 OK\r\n";
    std::string suffix = GetSuffix(path);
    mime_type = GetMimeType(suffix);
  }else {
    status = "HTTP/1.1 404 Not Found\r\n";
    content = ReadFile("wwwroot/404.html");
    mime_type = "text/html; charset=utf-8";
  }
  std::string resp = status;
  resp += "Content-Type: " + mime_type + "\r\n";
  resp += "Content-Length: " + std::to_string(content.size()) + "\r\n";
  resp += "Connection: close\r\n";
  resp += "\r\n";
  resp += content;
  return resp;

}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));

    // 将业务回调注入 Reactor 引擎
    EpollServer server(port, DynamicHttpHandler);

    server.Init();
    server.Start();

    return 0;
}