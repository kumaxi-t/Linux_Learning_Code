#include <iostream>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *routine(void *arg){
  char* name = static_cast<char*>(arg);

  while(true){
    pthread_mutex_lock(&mutex);

    std::cout << name << "   准备被挂起等待" << std::endl;

    pthread_cond_wait(&cond, &mutex);

    std::cout << name << "被唤醒，准备工作" << std::endl;

    pthread_mutex_unlock(&mutex);

    usleep(10000);
  }


  return nullptr;
}



int main(){
  pthread_t tid1, tid2, tid3;
  pthread_create(&tid1, nullptr, routine, (void*)"任务1");
  pthread_create(&tid2, nullptr, routine, (void*)"任务2");
  pthread_create(&tid3, nullptr, routine, (void*)"任务3");


  pthread_detach(tid1);
  pthread_detach(tid2);
  pthread_detach(tid3);  
  int count = 5;
  sleep(2);
  while(count--){

    std::cout << "\n";
    std::cout << "主线程发送唤醒信号， 随机唤醒一个出来干活" << std::endl;
    pthread_cond_signal(&cond);
    sleep(1);
  }

  return 0;
}