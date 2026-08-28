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

struct Session{
  std::string username;
  std::string login_time;
};

static std::unordered_map<std::string, Session> g_sessions;


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

  // _headers["Cookie"] : "session_id=X7k9P; theme=dark"   key: "session_id"
  std::string GetCookie(const std::string &key) const {
    auto it = _headers.find("Cookie");
    if(it == _headers.end()) return "";

    const std::string &cookie_str = it->second;
    std::string target = key + "=";
    auto pos = cookie_str.find(target);
    if(pos == std::string::npos) return "";

    size_t start = pos + target.size();
    auto end = cookie_str.find(";", start);
    if(end == std::string::npos) {
      return cookie_str.substr(start);
    }
    return cookie_str.substr(start, end - start);
  }

  // _body="username=hgtz&password=123"
  std::string GetBodyParam(const std::string &key) {
    if(_body.empty()) return "";

    std::stringstream ss(_body);
    std::string item;
    while(std::getline(ss, item, '&')) {
      auto pos = item.find('=');
      if(pos == std::string::npos) continue;
      std::string k = item.substr(0, pos);
      std::string v = item.substr(pos + 1);
      // 找到第一个不是 \r\n的下标
      size_t last = v.find_last_not_of(" \r\n");
      if(last != std::string::npos) {
        v = v.substr(0, last + 1);
      }
      if(k == key) {
        return v;
      }
    }
    return "";
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
class HttpResponse {
private:
  std::string _version;     // 协议版本 "HTTP/1.0"
  int _status_code;         // 状态码
  std::string _status_desc; // 状态描述 "OK"
  std::unordered_map<std::string, std::string> _headers;    // 响应报头哈希表
  std::string _body;        // 响应正文
public:
  HttpResponse()
  : _version("HTTP/1.0"),
    _status_code(200),
    _status_desc("OK") {
      SetHeaders("Connection", "close");
    }


  void SetStatus(int code, const std::string &desc) {
    _status_code = code;
    _status_desc = desc;
  }

  void SetHeaders(const std::string &key, const std::string &value) {
    _headers[key] = value;
  }

  void SetBody(const std::string &body, const std::string &mime_type = "text/html") {
    _body = body;
    SetHeaders("Content-Type", mime_type);
    SetHeaders("Content-Length", std::to_string(_body.size()));
  }

  std::string Serialize() const {
    std::string res;
    res += _version + gspace + std::to_string(_status_code) + gspace + _status_desc + glinespace;
    for(const auto [k, v] : _headers) {
      res += k + gsep + v + glinespace;
    }
    res += glinespace;
    res += _body;
    return res;
  }


  ~HttpResponse() {}
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

    HttpResponse resp;

    if(req.GetMethod() == "GET" && req.GetUrl() == "/login") {
      std::string sid = Util::GenerateSessionId();
      g_sessions[sid] = {"hgtz_admin", "1991-01-01"};
      std::string content = Util::GetFileContent(defaultwebpath + "/login.html");
      resp.SetStatus(200, "OK");
      resp.SetHeaders("Set-Cookie", "session_id=" + sid + "; Path=/; HttpOnly");
      resp.SetBody(content, "text/html");
      sock->Send(resp.Serialize());
      return ;
    }

    if(req.GetMethod() == "POST" && req.GetUrl() == "/login") {
      std::string username = req.GetBodyParam("username");
      std::string password = req.GetBodyParam("password");
      LOG(LogLevel::INFO) << "收到 POST 登录请求: 用户名=" << username << ", 密码=" << password;

      if(username == "hgtz" && password == "123456") {
        std::string sid = Util::GenerateSessionId();
        g_sessions[sid] = {username, "1991-01-01 16:00"};

        resp.SetStatus(302, "Found");
        resp.SetHeaders("Location", "/user");
        resp.SetHeaders("Set-Cookie", "session_id=" + sid + "; Path=/; HttpOnly");
      }else {
        resp.SetStatus(200, "OK");
        resp.SetBody("<html><h1>Login Failed: Wrong username or password!</h1><a href='/login'>Retry</a></html>");
      }
      sock->Send(resp.Serialize());
      return ;

    }
    if(req.GetUrl() == "/user") {
      std::string sid = req.GetCookie("session_id");
      auto it = g_sessions.find(sid);
      if(it == g_sessions.end() || sid.empty()) {
        resp.SetStatus(200, "OK");
        resp.SetBody("<html><h1>401 Unauthorized: Please visit /login first!</h1></html>");
      }else {
        // success
        std::string content = Util::GetFileContent(defaultwebpath + "/user.html");
        resp.SetStatus(200, "OK");
        resp.SetBody(content, "text/html");
      }
      sock->Send(resp.Serialize());
      return ;
    }

    // 处理重定向
    if(req.GetUrl() == "/bilibili") {
      resp.SetStatus(302, "Found");
      resp.SetHeaders("Location", "https://www.bilibili.com");
      sock->Send(resp.Serialize());
      return ;
    }
    // 处理静态资源读取
    std::string path = req.GetPath();
    std::string body = Util::GetFileContent(path);

    if(body.empty()) {
      // 文件不存在 -> 404
      std::string err_path = defaultwebpath + "/404.html";
      std::string err_body = Util::GetFileContent(err_path);
      if(err_body.empty()) {
        err_body = "<html><h1>404 Not Found</h1></html>";
      }
      resp.SetStatus(404, "Not Found");
      resp.SetBody(err_body);
    }else {
      //  文件存在
      std::string suffix = Util::GetSuffix(path);
      std::string mime_type = Util::GetMimeType(suffix);
      resp.SetStatus(200, "OK");
      resp.SetBody(body, mime_type);
    }
    sock->Send(resp.Serialize());


    // std::string path = req.GetPath();
    // std::string body = Util::GetFileContent(path);
    // std::string response;
    // if(body.empty()) {
    //   std::string err_path = defaultwebpath + "/404.html";
    //   std::string err_body = Util::GetFileContent(err_path);
    //   if (err_body.empty()) {
    //       err_body = "<html><h1>404 Not Found</h1></html>";
    //   }
    //   response += "HTTP/1.0 404 Not Found\r\n";
    //   response += "Content-Type: text/html\r\n";
    //   response += "Content-Length: " + std::to_string(err_body.size()) + "\r\n";
    //   response += "\r\n";
    //   response += err_body;
    //   sock->Send(response);  
    //   return ;
    // }
    // if(req.GetUrl() == "/bilibili") {
    //   response += "HTTP/1.0 302 Found\r\n";
    //   response += "Location: https://www.bilibili.com\r\n";
    //   response += "\r\n";
    //   sock->Send(response);
    //   return ;
    // }
    // std::string suffix = Util::GetSuffix(path);
    // std::string mime_type = Util::GetMimeType(suffix);
    // response += "HTTP/1.0 200 OK\r\n";
    // response += "Content-Type: " + mime_type + "\r\n";
    // response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    // response += "\r\n";
    // response += body;
    // sock->Send(response);
    // // response += "HTTP/1.0 200 OK\r\n";
    // // response += "Content-Type: text/html\r\n";
    // // response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    // // response += "\r\n"; 
    // // response += body;
    // // sock->Send(response);
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








