#include <iostream>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


void *routine(void* arg){
  char *name = static_cast<char*>(arg);

  pthread_mutex_lock(&mutex);
  std::cout << name << " 没活干休息一会" << std::endl;

  pthread_cond_wait(&cond, &mutex);

  
  std::cout << name << "来活了，起来干活了 " << std::endl;
  usleep(10000);

  pthread_mutex_unlock(&mutex);

  return nullptr;
}


int main(){

  pthread_t tid1, tid2, tid3;
  pthread_create(&tid1, nullptr, routine, (void*)"任务1");
  pthread_create(&tid2, nullptr, routine, (void*)"任务2");
  pthread_create(&tid3, nullptr, routine, (void*)"任务3");

  // 让你先休息一会
  sleep(3);


  std::cout << "来活了，全都别睡了，起来干活 " << std::endl;
  pthread_cond_broadcast(&cond);

  pthread_join(tid1, nullptr);
  pthread_join(tid2, nullptr);
  pthread_join(tid3, nullptr);
  return 0;
}