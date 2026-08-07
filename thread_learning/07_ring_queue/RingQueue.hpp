#include <pthread.h>
#include <unistd.h>
#include <vector>
#include "Sem.hpp"
#include <iostream>

template<class T>
class RingQueue{
private:
  std::vector<T> _ring_queue;
  int _cap;
  Sem _space_sem;
  Sem _data_sem;
  int _p_step;
  int _c_step;
  pthread_mutex_t _p_mutex;
  pthread_mutex_t _c_mutex;

public:
  RingQueue(int cap = 5) 
    : _ring_queue(cap), 
      _cap(cap),
      _space_sem(cap),
      _data_sem(0),
      _p_step(0),
      _c_step(0){
        pthread_mutex_init(&_p_mutex, nullptr);
        pthread_mutex_init(&_c_mutex, nullptr);
  }

  void Push(const T& in){
    // 先看有没有空位置，有就空位-1锁定一个位置，没有就挂起睡觉
    _space_sem.P();
    // 先订票再抢锁，避免两个人抢一个位置
    pthread_mutex_lock(&_p_mutex);

    // 生产过程
    _ring_queue[_p_step] = in;
    _p_step = (_p_step + 1) % _cap;

    // 解锁让给下一个人
    pthread_mutex_unlock(&_p_mutex);
    // 叫号通知消费者有数据了
    _data_sem.V();
  }

  void Pop(T* out){
    // 消费者先看有没有可以消费的数据
    _data_sem.P();
    // 确认有位置后抢锁确保只有一个位置只有一个消费者消费
    pthread_mutex_lock(&_c_mutex);
    
    // 消费过程
    *out = _ring_queue[_c_step];
    _c_step = (_c_step + 1) % _cap;

    // 消费完成把锁让开
    pthread_mutex_unlock(&_c_mutex);

    // 通知生产者去干活
    _space_sem.V();
  }


  ~RingQueue(){
    pthread_mutex_destroy(&_p_mutex);
    pthread_mutex_destroy(&_c_mutex);
  }

};