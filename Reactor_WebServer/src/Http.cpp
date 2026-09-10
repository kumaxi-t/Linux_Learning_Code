#include "../include/Http.hpp"



Http::Http(const std::string& webroot) {
  _webroot = webroot;
}

Http::~Http() {

}


std::string Http::HttpHandler(const std::string& req) {
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


