#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
int ticket = 20;
pthread_mutex_t mutex;


void* routine(void* arg){
  char* name = static_cast<char*>(arg);
  while(true){
    pthread_mutex_lock(&mutex);
    if(ticket > 0){
      printf("%s 成功卖出第 %d 张票\n", name, ticket);
      ticket--;
      pthread_mutex_unlock(&mutex);
    }else{
      pthread_mutex_unlock(&mutex);
      break;
    }
    usleep(1000);
    // 不加这个就被一个线程霸占了
  }

  return nullptr;
}


int main(){
  pthread_mutex_init(&mutex, nullptr);

  pthread_t t1, t2, t3;
  pthread_create(&t1, nullptr, routine, (void*)"seller1");
  pthread_create(&t2, nullptr, routine, (void*)"seller2");
  pthread_create(&t3, nullptr, routine, (void*)"seller3");




  pthread_join(t1, nullptr);
  pthread_join(t2, nullptr);
  pthread_join(t3, nullptr);


  pthread_mutex_destroy(&mutex);
  return 0;
}