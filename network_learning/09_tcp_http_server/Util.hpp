#pragma once
#include <string>
#include <fstream>
#include <iostream>



class Util {
public:
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










