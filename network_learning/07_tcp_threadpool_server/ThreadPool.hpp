#pragma once
#include "Comm.hpp"
#include "nocopy.hpp"

const static int gdefault_thread_num = 5;

template<class T>
class ThreadPool : public nocopy{
private:
  ThreadPool(int thread_num = gdefault_thread_num)
  : _thread_num(thread_num),
    _isrunning(false) {
      pthread_mutex_init(&_mutex, nullptr);
      pthread_cond_init(&_cond, nullptr);
  }


public:

  static ThreadPool<T> *GetInstance() {
    static ThreadPool<T> instance;
    return &instance;
  }


  void Lock() {
    pthread_mutex_lock(&_mutex);
  }
  void UnLock() {
    pthread_mutex_unlock(&_mutex);
  }
  void WakeUp () {
    pthread_cond_signal(&_cond);
  }
  void Wait() {
    pthread_cond_wait(&_cond, &_mutex);
  }
  bool IsEmpty() {
    return _task_queue.empty();
  }

  static void *ThreadRoutine(void *args) {
    pthread_detach(pthread_self());

    ThreadPool<T> *tp = static_cast<ThreadPool<T> *>(args);
    while(tp->_isrunning) {
      tp->Lock();

      while(tp->IsEmpty()) {
        tp->Wait();
      }
      T t = tp->Pop();
      tp->UnLock();
      t();
    } 
    return nullptr;
  }

  void Start() {
    if(_isrunning) return ;
    _isrunning = true;
    for(int i = 0; i < _thread_num; i++) {
      pthread_t tid;
      pthread_create(&tid, nullptr, ThreadRoutine, (void *)this);
    }
  }

  void Push(const T &task) {
    Lock();
    _task_queue.push(task);
    WakeUp();
    UnLock();
  }
  T Pop() {
    T t = _task_queue.front();
    _task_queue.pop();
    return t;
  }

  ~ThreadPool() {
    pthread_mutex_destroy(&_mutex);
    pthread_cond_destroy(&_cond);
  }



private:
  bool _isrunning;
  int _thread_num;
  std::queue<T> _task_queue;
  pthread_mutex_t _mutex;
  pthread_cond_t _cond;

};