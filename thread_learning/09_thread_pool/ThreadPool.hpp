#pragma once
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <queue>
#include "Cond.hpp"
#include "Log.hpp"
#include "Lock.hpp"
#include "Thread.hpp"
#include <functional>
namespace ThreadPoolModule{
  using Task = std::function<void()>;
  using namespace MutexModule;
  using namespace CondModule;
  using namespace LogModule;
  using namespace ThreadModule;
  static const int defaultnum = 3;
  static const int defaultcap = 2;
  template<class T = Task>
  class ThreadPool{
  private:
    std::queue<T> _task_queue;
    Mutex _mutex;
    Cond _c_cond;
    Cond _p_cond;
    std::vector<Thread<std::string>> _threads;
    int _thread_cnt;                      // 有几个工作线程
    bool _isrunning;                      // 是否在运行
    int _thread_wait_nums;                // 有多少工作线程在挂起等待任务
    int _producer_wait_nums;              // 有多少生产者等着位置生产
    static ThreadPool<T>* _instance;      // 唯一实例对象
    static Mutex _s_mutex;                // 静态锁
    int _max_cap;                         // 队列的最大容量

    ThreadPool(int num = defaultnum, int cap = defaultcap)
    : _thread_cnt(num),
      _isrunning(false),
      _thread_wait_nums(0),
      _producer_wait_nums(0),
      _max_cap(cap) {
    }

    ThreadPool(const ThreadPool<T> &) = delete;
    ThreadPool& operator=(const ThreadPool<T> &) = delete;


    void HandlerTask(std::string name) {
      while(true) {
        T task;
        {
          LockGuard lockguard(_mutex);
          while(_task_queue.empty() && _isrunning){
            _thread_wait_nums++;
            // 睡觉
            _c_cond.Wait(_mutex);

            // 醒了
            _thread_wait_nums--;
          }
          // 从循环中跳出来要么就是有任务了，要么就是线程池关闭了，首先得进行安全退出检查
          if(!_isrunning && _task_queue.empty()){
            // 线程池关闭了并且也没任务了，那就正常下班
            LOG(LogLevel::INFO) << "线程正常退出";
            break;
          }
          // 两种队列为空的情况都判断了，剩下的就是队列不为空的情况，要处理任务
          // 在锁内拿资源
          task = _task_queue.front();
          _task_queue.pop();
          if (_producer_wait_nums > 0) {
            _p_cond.Notify();
          }
        }  // <---- 临界区结束，出了这个大括号lockguard 自动析构，锁释放
        // 此时该任务已经释放其他线程看不见了只有当前线程拿到了，所以可以在临界区外面调用实现高并发
        task();
      }
    }
  public:

    static ThreadPool<T> *GetInstance() {
      // 双重检查锁定，如果用例已经创建好了就不需要再去抢锁避免系统调用开销      
      if(_instance == nullptr) {
        // 必须要用静态锁，进入这里时对象还没诞生，普通成员锁必须通过对象来访问，此时强制访问会发生段错误
        // 静态锁属于整个类本身，在程序刚启动（全局初始化阶段）就已经在内存的静态全局区分配好了，整个进程只有一把锁
        LockGuard lockguard(_s_mutex);
        if(_instance == nullptr) {
          _instance = new ThreadPool<T>;
          _instance->Start();
          return _instance;
        }
      }
      return _instance;
    }

    void Start() {  
      if(_isrunning) return ;
      _isrunning = true;
      for(int i = 1; i <= _thread_cnt; i++) {
        std::string name = "Thread-" + std::to_string(i);
        auto func = std::bind(&ThreadPool::HandlerTask, this, std::placeholders::_1);
        _threads.emplace_back(func, name, name);
      }
      for(auto &t : _threads){
        t.Start();
      }
      LOG(LogLevel::INFO) << "线程池启动成功，所有工作线程就位";
    }


    bool Enqueue(const T&task) {
      LockGuard lockguard(_mutex);
      if(!_isrunning) return false;
      // 队列满了并且还在运行，就等消费者先消费，再叫起来
      while (_task_queue.size() >= _max_cap) {
        _producer_wait_nums++;
        _p_cond.Wait(_mutex );
        _producer_wait_nums--;
      }
      // 如果睡醒了起来发现打样了那还上个毛班直接跑路
      if(!_isrunning) return false;

      _task_queue.emplace(task);
      if(_thread_wait_nums > 0) {
        // 有活了并且有人在睡觉，那就别睡了起来干活
        _c_cond.Notify();
      }
      return true;
    }

    void Stop(){
      {
        LockGuard lockguard(_mutex);
        if (!_isrunning)
          return;
        _isrunning = false;
        // 下班了所有人别睡了跑路了兄弟
        _c_cond.NotifyAll();
        _p_cond.NotifyAll();
      }
      for(auto &t : _threads) {
        t.Join();
      }
      LOG(LogLevel::INFO) << "线程池彻底关停，所有线程已回收！";
    }

    ~ThreadPool() {
      Stop();
    }
  };
  template<class T>
  ThreadPool<T> *ThreadPool<T>::_instance = nullptr;

  template<class T>
  Mutex ThreadPool<T>::_s_mutex;



}
// end ThreadPoolModule