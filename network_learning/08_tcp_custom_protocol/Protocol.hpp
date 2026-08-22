#pragma once
#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

namespace Protocol {
  const std::string LineBreakSep = "\r\n";

  std::string Encode(const std::string &message) {
    std::string len = std::to_string(message.size());
    // "44\r\n{\"datax\":10,\"datay\":20,\"oper\":43}\r\n"
    std::string package = len + LineBreakSep + message + LineBreakSep;
    return package;
  }

  bool Decode(std::string &package, std::string *message) {
    // 获取长度
    auto pos = package.find(LineBreakSep);
    // 没找着
    if(pos == std::string::npos) return false;
    // 长度字符串
    std::string len_str = package.substr(0, pos);
    // 长度大小
    int len = std::stoi(len_str);

    // "44\r\n{\"datax\":10,\"datay\":20,\"oper\":43}\r\n"
    // 总报文的长度
    int total_len = len_str.size() + len + 2 * LineBreakSep.size();
    // 没这么大说明还没接收完全
    if(package.size() < total_len) return false;

    // 数据收全了
    // 将数据提取出来
    *message = package.substr(len_str.size() + LineBreakSep.size(), len);
    // 把已经提取出来的报文抹除
    package.erase(0, total_len);
    return true;
  }



  // 请求类
  class Request {
  private:
    int _x;
    int _y;
    char _oper;


  public:
    Request() : _x(0), _y(0), _oper('+') {}
    Request(int x, int y, char oper) : _x(x), _y(y), _oper(oper) {}

    int GetX() const { return _x; };
    int GetY() const { return _y; };
    char GetOper() const { return _oper; };

    bool Serialize(std::string *out) {
      Json::Value root;
      root["x"] = _x;
      root["y"] = _y;
      root["oper"] = _oper;

      Json::FastWriter writer;
      *out = writer.write(root);
      return true;

    } 

    bool Deserialize(const std::string &in) {
      Json::Value root;
      Json::Reader reader;

      bool ok = reader.parse(in, root);
      if(!ok) return false;
      

      _x = root["x"].asInt();
      _y = root["y"].asInt();
      _oper = root["oper"].asInt();

      return true;
    }



  };


  // 响应类
  class Response {
  private:
    int _result;
    int _code;

  public:
    Response() : _result(0), _code(0) {}
    Response(int result, int code) : _result(result), _code(code) {} 

    int GetResult() const { return _result; };
    int GetCode() const { return _code; };


    bool Serialize(std::string *out) {
      Json::Value root;
      root["result"] = _result;
      root["code"] = _code;

      Json::FastWriter writer;
      *out = writer.write(root);
      return true;
    }

    bool Deserialize(const std::string &in) {
      Json::Value root;
      Json::Reader reader;

      bool ok = reader.parse(in, root);
      if(!ok) return false;

      _result = root["result"].asInt();
      _code = root["code"].asInt();
      return true;
    }

  };
}
// end Protocol