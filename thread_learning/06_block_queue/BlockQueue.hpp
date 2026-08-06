#pragma once

#include <iostream>
#include <unistd.h>
#include <queue>
#include <pthread.h>
#include "Lock.hpp"

using namespace MutexModule;
template<class T>
class BlockQueue{
private:
  std::queue<T> _block_queue;
  int _cap;
  Mutex _mutex;
  pthread_cond_t _product_cond;
  pthread_cond_t _consum_cond;

private:
  bool IsFull() const {
    return _block_queue.size() == _cap;
  }
  bool IsEmpty() const{
    return _block_queue.empty();
  }

public:
  BlockQueue(int cap = 5) : _cap(cap){
    pthread_cond_init(&_product_cond, nullptr);
    pthread_cond_init(&_consum_cond, nullptr);
  }
  ~BlockQueue(){
    pthread_cond_destroy(&_product_cond);
    pthread_cond_destroy(&_consum_cond);
  }

  void Enqueue(const T& in){
    LockGuard lockguard(_mutex);

    // 检查队列是否满了，这里要用while，用if只会被执行一次
    // 如果有多个线程被唤醒却只能执行一个那么另一个在队列满了之后就不会进行判断造成错误
    while(IsFull()){
      pthread_cond_wait(&_product_cond, _mutex.GetMutexOriginal());
    }

    _block_queue.push(in);

    // 把消费者叫起来，有货了进来消费
    pthread_cond_signal(&_consum_cond);

  }

  void Pop(T* out){
    LockGuard lockguard(_mutex);

    while(IsEmpty()){
      pthread_cond_wait(&_consum_cond, _mutex.GetMutexOriginal());
    }

    *out = _block_queue.front();

    _block_queue.pop();

    // 把生产者叫起来，没货了去补货
    pthread_cond_signal(&_product_cond);
  }

};


