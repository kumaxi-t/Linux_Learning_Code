#pragma once
#include <pthread.h>
#include <iostream>
namespace MutexModule{

class Mutex{
public:
  Mutex(){
    pthread_mutex_init(&_mutex, nullptr);
  }
  void Lock(){
    int n = pthread_mutex_lock(&_mutex);
  }
  void Unlock(){
    int n = pthread_mutex_unlock(&_mutex);
  }  
  ~Mutex(){
    pthread_mutex_destroy(&_mutex);
  }
private:
  pthread_mutex_t _mutex;
};

class LockGuard{
public:
  LockGuard(Mutex& mutex) : _mutex(mutex){
    _mutex.Lock();
  }
  ~LockGuard(){
    _mutex.Unlock();
  }

private:
  Mutex& _mutex;
};

}