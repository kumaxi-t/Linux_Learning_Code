#pragma once
#include "Comm.hpp"
#include "InetAddr.hpp"
#include "Lock.hpp"
#include "TcpServer.hpp"
#include "Socket.hpp"
#include "Util.hpp"
#include "Log.hpp"
using namespace LogModule;
using namespace Net_Work;
const std::string gspace = " ";
const std::string glinespace = "\r\n";
const std::string gsep = ": ";
const std::string defaultwebpath = "wwwroot";

class HttpRequest {
public:
  HttpRequest() {

  }

  std::string GetMethod() const { return _method; };
  std::string GetUrl() const { return _url; };
  std::string GetBody() const { return _body; };
  bool Deserialize(std::string &reqstr) {
    // 获取第一行请求行
    // "GET /index.html HTTP/1.1"
    std::string reqline = GetOneLine(reqstr);
    if(reqline.empty()) return false;

    // 将请求行拆解到 _method, _url, _version
    std::stringstream ss(reqline);
    ss >> _method >> _url >> _version;
    // 循环解析所有Header K/V 对
    while(true) {
      // Host: 127.0.0.1:8080
      std::string line = GetOneLine(reqstr);
      if(line.empty()) {
        _blankline = glinespace;
        break;
      }
      auto pos = line.find(gsep);
      if(pos == std::string::npos) continue;
      std::string key = line.substr(0, pos);
      std::string value = line.substr(pos + gsep.size());
      _headers[key] = value;
    }
    _body = reqstr;
    return true;
  }


  void PrintDebug() {
    LOG(LogLevel::DEBUG) << "\n--- [HttpRequest 解析结果] ---\n"
                         << "Method: " << _method << "\n"
                         << "URL: " << _url << "\n"
                         << "Version: " << _version;
    for (auto &kv : _headers) {
        LOG(LogLevel::DEBUG) << "Header -> " << kv.first << ": " << kv.second;
    }
    LOG(LogLevel::DEBUG) << "Body: " << _body << "\n-----------------------------";

  }

  std::string GetPath() {
    if(_url == "/" || _url.empty()) return defaultwebpath + "/index.html";
    return defaultwebpath + _url; 
  }

  ~HttpRequest() {

  }
private:
  std::string GetOneLine(std::string &reqstr) {
    // 查找 glinespace (\r\n) 在 reqstr 中的位置
    auto pos = reqstr.find(glinespace);
    // 如果没找到，说明不是完整的行，返回空串 ""
    if(pos == std::string::npos) return "";

    // 截取从 0 到 pos 的字符串 line（不包含 \r\n）
    std::string line = reqstr.substr(0, pos);
    // 从 reqstr 中抹除这一行以及末尾的 glinespace
    reqstr.erase(0, line.size() + glinespace.size());
    // 返回提取好的这一行
    return line;
  }

private:
  std::string _method;      // 请求方法  "GET" / "POST"
  std::string _url;         // 请求路径  "/index.html"
  std::string _version;     // HTTP协议版本  "HTTP/1.1"

  std::unordered_map<std::string, std::string> _headers;  // 请求报头哈希表  "Host": "127.0.0.1:8080"
  std::string _blankline;   // 空行   "\r\n"

  std::string _body;        // 请求正文body   "user=hgtz&pass=123"


};

class Http {
public:
  Http(uint16_t port) : _tsvr(std::make_unique<TcpServer>(port)) {

  }
  void HandlerHttpRequest(std::shared_ptr<Socket> &sock, InetAddr &client) {
    std::string httpreqstr;

    bool ok = sock->Recv(&httpreqstr, 4096);
    if(!ok || httpreqstr.empty()) return ;
    HttpRequest req;
    if(!req.Deserialize(httpreqstr)) {
      LOG(LogLevel::WARNING) << "Deserialize HTTP Requset failed";
      return ;
    }
    req.PrintDebug();

    std::string path = req.GetPath();
    std::string body = Util::GetFileContent(path);
    std::string response;
    response += "HTTP/1.0 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    response += "\r\n"; 
    response += body;
    sock->Send(response);
  }

  void Start() {
    _tsvr->Start([this](std::shared_ptr<Socket> &sock, InetAddr &client){
      this->HandlerHttpRequest(sock, client);
    });
  }
  ~Http() {

  }

private:
  std::unique_ptr<TcpServer> _tsvr;
};








