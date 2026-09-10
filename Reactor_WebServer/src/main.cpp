#include <iostream>
#include <string>
#include <algorithm>
#include "../include/Business.hpp"
#include "../include/EpollServer.hpp"
#include "../include/Http.hpp"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));

    // 将业务回调注入 Reactor 引擎

    Http http_handler;
    auto business_cb = std::bind(&Http::HttpHandler, &http_handler, std::placeholders::_1);

    EpollServer server(port, business_cb);

    server.Init();
    server.Start();

    return 0;
}