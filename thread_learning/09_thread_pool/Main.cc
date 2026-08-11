#include <iostream>
#include <thread>
#include <unistd.h>
#include "ThreadPool.hpp"
using namespace ThreadPoolModule;
// 模拟任务
void DownloadTask(int task_id) {
    LOG(LogLevel::DEBUG) << "  ===> [任务 " << task_id << "] 开始下载，处理线程: " 
              << std::this_thread::get_id();
    usleep(200000); // 模拟耗时 200ms
}


void HeavyTask(int task_id) {
    LOG(LogLevel::DEBUG) << "  ===> [任务 " << task_id << "] 开始处理，处理线程: " 
              << std::this_thread::get_id();
    sleep(1); // 模拟耗时任务
}

int main(){
  ENABLE_CONSOLE_LOG();

  // ThreadPool<Task> pool(5);
  // pool.Start();
  // for (int i = 1; i <= 10; ++i) {
  //   pool.Enqueue([i]() { DownloadTask(i); });
  //   usleep(50000); 
  // }
  //
  // sleep(2);
  // 
  // pool.Stop();

  // for (int i = 1; i <= 10; i++) {
  //   ThreadPool<Task>::GetInstance()->Enqueue([i](){
  //     DownloadTask(i);
  //   });
  //   usleep(50000);
  // }

  for (int i = 1; i <= 10; ++i) {
    LOG(LogLevel::DEBUG) << "[Main] 准备投递任务: " << i;
    ThreadPool<Task>::GetInstance()->Enqueue([i]() { 
      HeavyTask(i); 
    });
    LOG(LogLevel::DEBUG) << "[Main] 成功投递任务: " << i;
  }

  sleep(2);
  ThreadPool<Task>::GetInstance()->Stop();

  return 0;
}