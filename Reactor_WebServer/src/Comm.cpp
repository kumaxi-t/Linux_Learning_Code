#include "../include/Comm.hpp"


bool SetNonBlock(int fd) {
  int flags = fcntl(fd, F_GETFL);
  if(flags < 0) {
    std::cerr << "fcntl error" << std::endl;
    return false;
  }
  if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    std::cerr << "fcntl error" << std::endl;
    return false;
  }
  return true;
}

std::string ReadFile(const std::string& path) {
  std::ifstream file(path, std::ios::binary);

  if(!file.is_open()) {
    return "";
  }
  std::stringstream ss;
  ss << file.rdbuf();
  return ss.str();
}

std::string GetSuffix(const std::string& path) {
  auto pos = path.find(".");
  if(pos == std::string::npos) {
    return ".html";
  }
  return path.substr(pos);
}


std::string GetMimeType(const std::string& suffix) {
    static const std::unordered_map<std::string, std::string> mime_map = {
        {".html", "text/html; charset=utf-8"},
        {".htm",  "text/html; charset=utf-8"},
        {".css",  "text/css"},
        {".js",   "application/javascript"},
        {".png",  "image/png"},
        {".jpg",  "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif",  "image/gif"},
        {".ico",  "image/x-icon"},
        {".txt",  "text/plain; charset=utf-8"}
    };

    auto it = mime_map.find(suffix);
    if (it != mime_map.end()) {
        return it->second;
    }
    return "application/octet-stream"; // 未知类型默认当作二进制流
}