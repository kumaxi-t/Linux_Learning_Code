#pragma once
#include "Comm.hpp"
#include "Http.hpp"


// 定义一个业务回调函数将输入转成大写
std::string ToUpperService(const std::string& req);



std::string SimpleHttpHandler(const std::string& req);


void AsyncProcessHttpRequest(int fd, std::string req);
