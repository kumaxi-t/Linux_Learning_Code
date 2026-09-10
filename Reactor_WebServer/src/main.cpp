#include <iostream>
#include <string>
#include <algorithm>
#include "../include/EpollServer.hpp"

// 定义一个业务回调函数将输入转成大写
std::string ToUpperService(const std::string& req) {
    std::string resp = req;
    for (char& c : resp) {
        c = std::toupper(c);
    }
    return "[Server Echo]: " + resp;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));

    // 将业务回调注入 Reactor 引擎
    EpollServer server(port, ToUpperService);

    server.Init();
    server.Start();

    return 0;
}