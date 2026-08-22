#pragma once 
#include "Protocol.hpp"

// 业务模块
namespace Business {

  Protocol::Response Execute(const Protocol::Request &req) {
    int x = req.GetX();
    int y = req.GetY();
    char oper = req.GetOper();

    int result = 0;
    int code = 0;


    if(oper == '+') {
      result = x + y;
    }else if (oper == '-') {
      result = x - y;
    }else if (oper == '*') {
      result = x * y;
    }else if (oper == '/') {
      if(y == 0) {
        code = 1; // 除0错误
      }else {
        result = x / y;
      }
    }else if (oper == '%') {
      if(y == 0) {
        code = 2; // 模0错误
      }else {
        result = x % y;
      }
    }else {
      code = 3; // 未知运算符
    }
    return Protocol::Response(result, code);
  }


}
// end Business


