#include "../include/Http.hpp"



Http::Http(const std::string& webroot) {
  _webroot = webroot;
}

Http::~Http() {

}


std::string Http::HttpHandler(const std::string& req) {
    // 提取请求行
    auto line_end = req.find("\r\n");
    if (line_end == std::string::npos) return "";
    std::string req_line = req.substr(0, line_end);

    std::string method, url, version;
    std::stringstream ss(req_line);
    ss >> method >> url >> version;

    // 文件上传 (POST /upload)
    if (method == "POST" && url == "/upload") {
        // 找到请求头和正文的分界线
        auto body_start = req.find("\r\n\r\n");
        if (body_start != std::string::npos) {
            std::string body = req.substr(body_start + 4);

            // 解析表单里的文件名与真实二进制数据（这里做极简提取，直接保存在 wwwroot 目录下）
            std::string save_filename = "uploaded_file.dat"; // 缺省文件名
            auto fn_pos = body.find("filename=\"");
            if (fn_pos != std::string::npos) {
                auto fn_end = body.find("\"", fn_pos + 10);
                if (fn_end != std::string::npos) {
                    save_filename = body.substr(fn_pos + 10, fn_end - (fn_pos + 10));
                }
            }

            // 提取表单数据中文件的真实起始与结束
            auto file_data_start = body.find("\r\n\r\n", fn_pos);
            if (file_data_start != std::string::npos) {
                file_data_start += 4;
                // 表单末尾会带有边界分界线，找到最后一个换行符作为截断
                auto file_data_end = body.rfind("\r\n--");
                if (file_data_end == std::string::npos || file_data_end <= file_data_start) {
                    file_data_end = body.size();
                }

                // 调用 Linux 系统调用将文件写入磁盘
                std::string full_path = _webroot + "/" + save_filename;
                int disk_fd = ::open(full_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (disk_fd >= 0) {
                    const char* data_ptr = body.data() + file_data_start;
                    size_t data_size = file_data_end - file_data_start;
                    ::write(disk_fd, data_ptr, data_size);
                    ::close(disk_fd);
                }
            }
        }

        // 上传成功后，返回一个 303 重定向，让浏览器自动跳回首页刷新出新文件列表
        std::string resp = "HTTP/1.1 303 See Other\r\n";
        resp += "Location: /\r\n";
        resp += "Content-Length: 0\r\n";
        resp += "Connection: close\r\n";
        resp += "\r\n";
        return resp;
    }

    // 首页动态云盘列表 (GET /)
    if (url == "/" || url == "/index.html") {
        std::string content = BuildDirectoryHtml(_webroot);
        std::string resp = "HTTP/1.1 200 OK\r\n";
        resp += "Content-Type: text/html; charset=utf-8\r\n";
        resp += "Content-Length: " + std::to_string(content.size()) + "\r\n";
        resp += "Connection: close\r\n";
        resp += "\r\n";
        resp += content;
        return resp;
    }

    // 常规文件下载与访问 (GET /xxx)
    std::string path = _webroot + url;
    std::string content = ReadFile(path);
    std::string status;
    std::string mime_type;

    if (!content.empty()) {
        status = "HTTP/1.1 200 OK\r\n";
        std::string suffix = GetSuffix(path);
        mime_type = GetMimeType(suffix);
    } else {
        status = "HTTP/1.1 404 Not Found\r\n";
        content = ReadFile(_webroot + "/404.html");
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


