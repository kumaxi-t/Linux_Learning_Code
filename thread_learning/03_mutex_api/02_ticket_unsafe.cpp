#include <iostream>
#include <unistd.h>
#include <pthread.h>
int ticket = 20;
void* routine(void* arg){
  char* name = static_cast<char*>(arg);
  while(1){
    if(ticket > 0){
      std::cout << name << ": " << ticket << std::endl;
      // ticket 出现了负数
      ticket--;
    }else{
      break;
    }
    usleep(1000);
  }
  return nullptr;
}


int main(){
  pthread_t tid1, tid2, tid3;
  pthread_create(&tid1, nullptr, routine, (void*)"op1");
  pthread_create(&tid2, nullptr, routine, (void*)"op2");
  pthread_create(&tid3, nullptr, routine, (void*)"op3");

  pthread_join(tid1, nullptr);
  pthread_join(tid2, nullptr);
  pthread_join(tid3, nullptr);

  return 0;
}