#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <unordered_map>
static const std::unordered_map<std::string, std::string> mime_map = {
    {".html", "text/html"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".txt", "text/plain"}
  };

class Util {
public:

  // 提取后缀
  static std::string GetSuffix(const std::string &path) {
    auto pos = path.rfind('.');
    if(pos == std::string::npos) return ".html";
    return path.substr(pos);
  }

  // 后缀到 Content-Type 的哈希映射
  static std::string GetMimeType(const std::string &suffix) {
    auto it = mime_map.find(suffix);
    if(it == mime_map.end()) return "text/html";
    return it->second;
  }


  static std::string GetFileContent(const std::string &path) {
    std::ifstream in(path, std::ios::binary);
    if(!in.is_open()) return "";

    in.seekg(0, in.end);
    int filesize = in.tellg();
    in.seekg(0, in.beg);
    std::string content;
    content.resize(filesize);
    in.read((char *)content.c_str(), filesize);
    in.close();
    return content;
  }
  

};










