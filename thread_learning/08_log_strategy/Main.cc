#include "Log.hpp"

using namespace LogModule;

void TestLog() {
  int a = 100;
  double b = 3.14;
  LOG(LogLevel::DEBUG) << "调试模式开启，a = " << a;
  LOG(LogLevel::INFO) << "系统正常运行中，b = " << b;
  LOG(LogLevel::WARNING) << "发现潜在风险！";
  LOG(LogLevel::ERROR) << "网络连接断开！";
  LOG(LogLevel::FATAL) << "内存溢出，程序即将崩溃！";
}

int main() {
  std::cout << "=== 测试 1：打印到屏幕 ===" << std::endl;
  ENABLE_CONSOLE_LOG();
  TestLog();

  std::cout << "\n=== 测试 2：切换策略，写入文件 ===" << std::endl;
  ENABLE_FILE_LOG();
  TestLog();
  
  std::cout << "文件测试完成，请检查 ./log/log.txt" << std::endl;
  return 0;
}