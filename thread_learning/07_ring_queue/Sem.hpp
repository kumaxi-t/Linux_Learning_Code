#include <iostream>
#include <semaphore.h>


class Sem{
private:
  sem_t _sem;

public:

  Sem(const Sem&) = delete;
  Sem& operator=(const Sem&) = delete;
  Sem(int value){
    sem_init(&_sem, 0, value);
  }
  ~Sem(){
    sem_destroy(&_sem);
  }
  void P(){
    sem_wait(&_sem);
  }
  void V(){
    sem_post(&_sem);
  }

};