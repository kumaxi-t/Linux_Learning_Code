#ifndef _THREAD_H_
#define _THREAD_H_
#include <iostream>
#include <pthread.h>
#include <string>
#include <cstring>
#include <functional>
namespace ThreadModule {
  std::uint32_t cnt = 0;
template<typename T>
class Thread {
  using func_t = std::function<void(T)>; 
private:
  void EnableDetach() { _isdetach = true; }

public:
  Thread(func_t func, T data)  
    : _tid(0),
     _isdetach(false), 
     _isrunning(false),
     _res(nullptr),
     _func(func),
     _data(data) {
    _name = "Thread-" + std::to_string(cnt++);
  }

  void Detach() {
    if (_isdetach)
      return;
    if (_isrunning)
      pthread_detach(_tid);
    EnableDetach();
  }

  void EnableRunning() { _isrunning = true; }


  static void *Routine(void *arg){
    Thread *self = static_cast<Thread*>(arg);
    self->EnableRunning();
    if (self->_isdetach) {
      self->Detach();
    }
    self->_func(self->_data);
    return nullptr;
  }
  bool Start() {
    int n = pthread_create(&_tid, nullptr, Routine, this);
    if (n > 0) {
      std::cerr << "Create thread error" << strerror(n) << std::endl;
      return false;
    }else{
      std::cout << _name << "create success" << std::endl;
    }
    return true;
  }

  bool Stop() {
    if (_isrunning) {
      int n = pthread_cancel(_tid);
      if (n != 0) {
        std::cerr << "Stop thread error" << strerror(n) << std::endl;
        return false;
      }else{
        _isrunning = false;
        std::cout << _name << "Stop" << std::endl;
        return true;
      }
    }
    return false;
  }

  void Join() {
    if(_isdetach) return ;
    int n = pthread_join(_tid, &_res);
    if (n != 0) {
      std::cerr << "Join thread error" << strerror(n) << std::endl;
    }else {
      std::cout << "join success" << std::endl;
    }
  }

  ~Thread() {}

private:
  pthread_t _tid;
  std::string _name;
  bool _isdetach;
  bool _isrunning;
  void *_res;
  func_t _func;
  T _data;
};

} // namespace ThreadModule

#endif
