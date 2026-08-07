#include "RingQueue.hpp"
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <pthread.h>

void *ProducerRoutine(void *arg){

  RingQueue<int> *bq = static_cast<RingQueue<int>*>(arg);

  while(true){

    int data = rand() % 100 + 1;

    bq->Push(data);

    std::cout << "生产者产生任务： " << data << std::endl;

    usleep(200000);
  }


  return nullptr;
}

void *ConsumerRoutine(void *arg){
 
  RingQueue<int> *rq = static_cast<RingQueue<int>*>(arg); 

  while(true){
    int data = 0;
    rq->Pop(&data);
    
    std::cout << "消费者消费： " << data << std::endl;
    sleep(1);

  }
}



int main(){
  srand(time(nullptr));

  RingQueue<int> *rq = new RingQueue<int> (5);

  pthread_t p[3], c[3];
  for (int i = 0; i < 3; ++i) {
    pthread_create(&p[i], nullptr, ProducerRoutine, rq);
    pthread_create(&c[i], nullptr, ConsumerRoutine, rq);
  }

  for (int i = 0; i < 3; ++i) {
    pthread_join(p[i], nullptr);
    pthread_join(c[i], nullptr);
  }

  delete rq;

  return 0;
}