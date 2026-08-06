#include "BlockQueue.hpp"
#include <ctime>
#include <cstdlib>
#include <unistd.h>
void *ProducerRoutine(void *arg){

  BlockQueue<int> *bq = static_cast<BlockQueue<int>*>(arg);

  while(true){

    int data = rand() % 100 + 1;

    bq->Enqueue(data);

    std::cout << "生产者产生任务： " << data << std::endl;

    usleep(200000);
  }


  return nullptr;
}

void *ConsumerRoutine(void *arg){
 
  BlockQueue<int> *bq = static_cast<BlockQueue<int>*>(arg); 

  while(true){
    int data = 0;
    bq->Pop(&data);
    
    std::cout << "消费者消费： " << data << std::endl;
    sleep(1);

  }
}



int main(){
  srand(time(nullptr));

  BlockQueue<int> *bq = new BlockQueue<int> (5);

  pthread_t p, q;
  pthread_create(&p, nullptr, ProducerRoutine, bq);
  pthread_create(&q, nullptr, ConsumerRoutine, bq);  


  pthread_join(p, nullptr);
  pthread_join(q, nullptr);

  delete bq;


  return 0;
}