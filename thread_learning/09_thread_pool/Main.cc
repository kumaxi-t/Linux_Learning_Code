#include <iostream>
#include <thread>
#include <unistd.h>
#include "ThreadPool.hpp"
using namespace ThreadPoolModule;
// 模拟任务
void DownloadTask(int task_id) {
    std::cout << "  ===> [任务 " << task_id << "] 开始下载，处理线程: " 
              << std::this_thread::get_id() << std::endl;
    usleep(200000); // 模拟耗时 200ms
}

int main(){
  ENABLE_CONSOLE_LOG();

  ThreadPool<Task> pool(5);
  pool.Start();
  for (int i = 1; i <= 10; ++i) {
    pool.Enqueue([i]() { DownloadTask(i); });
    usleep(50000); 
  }

  sleep(2);

  pool.Stop();
  return 0;
}