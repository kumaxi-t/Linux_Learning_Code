#include <iostream>
#include <pthread.h>
#include <unistd.h>
void* routine(void* arg){
  char* name = static_cast<char*>(arg);
  for(int i = 0; i < 3; i++){
    std::cout << "我系" << name << ", 正在运行中  " << i << std::endl;
    // 输出是乱的，这一行代码分多次执行，在线程并发的情况下字符会交错打乱
    sleep(1); 
  }
  return nullptr;
}




int main() {
  pthread_t t1, t2;
  pthread_create(&t1, nullptr, routine, (void*)"thread-A");
  pthread_create(&t2, nullptr, routine, (void*)"thread-B");

  pthread_join(t1, nullptr);
  pthread_join(t2, nullptr);
  return 0;
}