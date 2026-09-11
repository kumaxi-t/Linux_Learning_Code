#include "../include/Business.hpp"




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
void AsyncProcessHttpRequest(int fd, std::string req) {
    Http httphandler;
    std::string resp = httphandler.HttpHandler(req);

    if (!resp.empty()) {
        size_t total_sent = 0;
        size_t to_send = resp.size();
        const char* buf = resp.c_str();

        while (total_sent < to_send) {
            ssize_t s = write(fd, buf + total_sent, to_send - total_sent);
            if (s > 0) {
                total_sent += s;
            } else {
                if (errno == EINTR) {
                    continue; // 被信号打断，立即重试
                }
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // 内核发送缓冲区满了，微睡 1 毫秒（1000 微秒）等待网卡发走一部分数据，继续写
                    usleep(1000);
                    continue;
                }
                // 只有遇到对端关闭或严重错误才退出
                break;
            }
        }
    }
    close(fd);
}




static std::string FormatFileSize(size_t bytes) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1);
    if (bytes < 1024) {
        oss << bytes << " B";
    } else if (bytes < 1024 * 1024) {
        oss << (bytes / 1024.0) << " KB";
    } else {
        oss << (bytes / (1024.0 * 1024.0)) << " MB";
    }
    return oss.str();
}

std::string BuildDirectoryHtml(const std::string& dir_path) {
    DIR* dir = opendir(dir_path.c_str());
    if (!dir) {
        return "<html><body><h1>无法打开存储目录</h1></body></html>";
    }

    std::string html;
    html += "<!DOCTYPE html>";
    html += "<html lang='zh-CN'>";
    html += "<head>";
    html += "  <meta charset='utf-8'>";
    html += "  <meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "  <title>CloudDrive - 私人云盘</title>";
    html += "  <style>";
    html += "    :root { --primary: #4f46e5; --primary-hover: #4338ca; --bg: #f8fafc; --card: #ffffff; --text: #1e293b; --subtext: #64748b; }";
    html += "    body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; background-color: var(--bg); color: var(--text); margin: 0; padding: 40px 20px; }";
    html += "    .container { max-width: 800px; margin: 0 auto; }";
    html += "    .header { text-align: center; margin-bottom: 30px; }";
    html += "    .header h1 { font-size: 28px; margin: 0 0 10px 0; color: #0f172a; font-weight: 700; letter-spacing: -0.5px; }";
    html += "    .header p { color: var(--subtext); margin: 0; font-size: 14px; }";
    
    // 上传区域卡片
    html += "    .card { background: var(--card); border-radius: 16px; box-shadow: 0 4px 20px -2px rgba(0, 0, 0, 0.05); padding: 24px; margin-bottom: 24px; border: 1px solid #edf2f7; }";
    html += "    .upload-box { border: 2px dashed #cbd5e1; border-radius: 12px; padding: 28px 20px; text-align: center; background: #fdfdfd; transition: all 0.2s ease; cursor: pointer; }";
    html += "    .upload-box:hover { border-color: var(--primary); background: #f5f3ff; }";
    html += "    .file-input { display: none; }";
    html += "    .upload-btn { background: var(--primary); color: white; border: none; padding: 10px 24px; border-radius: 8px; font-size: 14px; font-weight: 600; cursor: pointer; transition: background 0.2s ease; margin-top: 12px; }";
    html += "    .upload-btn:hover { background: var(--primary-hover); }";
    html += "    .upload-tip { font-size: 13px; color: var(--subtext); margin-top: 8px; }";

    // 文件列表样式
    html += "    .list-header { display: flex; justify-content: space-between; align-items: center; padding-bottom: 12px; border-bottom: 1px solid #f1f5f9; margin-bottom: 12px; font-weight: 600; font-size: 14px; color: var(--subtext); }";
    html += "    .file-item { display: flex; justify-content: space-between; align-items: center; padding: 14px 12px; border-radius: 8px; transition: background 0.2s ease; border-bottom: 1px solid #f8fafc; }";
    html += "    .file-item:hover { background: #f1f5f9; }";
    html += "    .file-info { display: flex; align-items: center; gap: 12px; }";
    html += "    .file-icon { font-size: 20px; line-height: 1; }";
    html += "    .file-name { font-size: 15px; font-weight: 500; color: #1e293b; text-decoration: none; word-break: break-all; }";
    html += "    .file-name:hover { color: var(--primary); }";
    html += "    .file-size { font-size: 13px; color: var(--subtext); font-variant-numeric: tabular-nums; }";
    html += "    .action-btn { font-size: 12px; color: var(--primary); background: #e0e7ff; padding: 6px 12px; border-radius: 6px; text-decoration: none; font-weight: 600; transition: all 0.2s; }";
    html += "    .action-btn:hover { background: var(--primary); color: white; }";
    html += "  </style>";
    html += "</head>";
    html += "<body>";
    html += "  <div class='container'>";
    
    // 页面头部
    html += "    <div class='header'>";
    html += "      <h1>⚡ My CloudDrive</h1>";
    html += "      <p>基于 C++ Reactor 异步驱动的高性能私有网盘</p>";
    html += "    </div>";

    // 上传卡片
    html += "    <div class='card'>";
    html += "      <form id='upload-form' action='/upload' method='POST' enctype='multipart/form-data'>";
    html += "        <label class='upload-box' style='display:block;'>";
    html += "          <span style='font-size: 32px;'>☁️</span>";
    html += "          <div style='font-weight: 600; margin-top: 8px;'>点击这里选择要上传的文件</div>";
    html += "          <div id='selected-name' class='upload-tip'>支持图片、代码、压缩包等各类格式文件</div>";
    html += "          <input type='file' name='file' class='file-input' required onchange=\"document.getElementById('selected-name').innerText = '已选择: ' + this.files[0].name;\" />";
    html += "        </label>";
    html += "        <div style='text-align: right;'>";
    html += "          <button type='submit' class='upload-btn'>开始上传</button>";
    html += "        </div>";
    html += "      </form>";
    html += "    </div>";

    // 文件列表卡片
    html += "    <div class='card'>";
    html += "      <div class='list-header'>";
    html += "        <span>文件名称</span>";
    html += "        <span>操作与大小</span>";
    html += "      </div>";

    // 遍历目录
    struct dirent* entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        if (std::string(entry->d_name) == "." || std::string(entry->d_name) == "..") {
            continue;
        }

        if (entry->d_type == DT_REG) {
            std::string filename = entry->d_name;
            std::string filepath = dir_path + "/" + filename;

            // 获取文件大小
            struct stat st;
            size_t file_size = 0;
            if (stat(filepath.c_str(), &st) == 0) {
                file_size = st.st_size;
            }

            // 根据文件名后缀分配轻量图标
            std::string icon = "📄";
            if (filename.find(".png") != std::string::npos || filename.find(".jpg") != std::string::npos || filename.find(".jpeg") != std::string::npos) {
                icon = "🖼️";
            } else if (filename.find(".cpp") != std::string::npos || filename.find(".hpp") != std::string::npos || filename.find(".h") != std::string::npos) {
                icon = "💻";
            } else if (filename.find(".tar") != std::string::npos || filename.find(".zip") != std::string::npos || filename.find(".gz") != std::string::npos) {
                icon = "📦";
            }

            html += "      <div class='file-item'>";
            html += "        <div class='file-info'>";
            html += "          <span class='file-icon'>" + icon + "</span>";
            html += "          <a class='file-name' href='/" + filename + "' target='_blank'>" + filename + "</a>";
            html += "        </div>";
            html += "        <div style='display: flex; align-items: center; gap: 14px;'>";
            html += "          <span class='file-size'>" + FormatFileSize(file_size) + "</span>";
            html += "          <a class='action-btn' href='/" + filename + "' download>下载</a>";
            html += "        </div>";
            html += "      </div>";
        }
    }
    closedir(dir);

    html += "    </div>";
    html += "  </div>";
    html += "</body>";
    html += "</html>";
    return html;
}