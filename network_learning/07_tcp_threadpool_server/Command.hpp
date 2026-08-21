#pragma once
#include "Comm.hpp"





class Command {
public:

  Command() {
    _blacklist = {"rm", "kill", "unlink", "shutdown", "reboot", "mv", "top"};
  }

  bool SafeCheck(const std::string &cmd) {
    for(auto s : _blacklist) {
      if(cmd.find(s) != std::string::npos) return false;
    }
    return true;
  }

  std::string Execute(const std::string &cmd) {
    if(!SafeCheck(cmd)) {
      return "Unsafe command refused.\n";
    }
    FILE *fp = popen(cmd.c_str(), "r");
    if(fp == nullptr) {
      return "popen failed.\n";
    }

    std::string result;
    char buffer[64];
    
    while(fgets(buffer, sizeof(buffer), fp) != nullptr) {
      result += buffer;
    }
    
    pclose(fp);

    return result.empty() ? "[execute done, no output]\n" : result;

  }

  ~Command() {}

private:
  std::vector<std::string> _blacklist;

};