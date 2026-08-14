#include <iostream>
#include <string>
#include <unordered_map>
#include "UdpServer.hpp"
#include <memory>

void TranslateDict(const std::string &req, std::string *resp) {
    static std::unordered_map<std::string, std::string> dict = {
        {"apple", "苹果"},
        {"banana", "香蕉"},
        {"hello", "你好"},
        {"dog", "狗"}
    };

    auto iter = dict.find(req);
    if (iter != dict.end()) {
        *resp = iter->second;
    } else {
        *resp = "Unknown (未查到该词)";
    }
}


int main() {

    std::unique_ptr<UdpServer> svr = std::make_unique<UdpServer>(TranslateDict, 8888);

    svr->Init();
    svr->Start();



    return 0;
}