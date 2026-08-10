#pragma once

#include <pthread.h>
#include "Lock.hpp" 

namespace CondModule {
    using namespace MutexModule;

    class Cond {
    public:
        Cond() {
            int n = pthread_cond_init(&_cond, nullptr);
            (void)n; 
        }

        // 禁止拷贝和赋值
        Cond(const Cond&) = delete;
        Cond& operator=(const Cond&) = delete;

        // 让线程在当前条件变量上挂起等待
        // 传入我们封装的 Mutex 对象，内部调用 GetMutexOriginal() 获取原生指针
        void Wait(Mutex &mutex) {
            int n = pthread_cond_wait(&_cond, mutex.GetMutexOriginal());
            (void)n;
        }

        // 唤醒一个等待的线程
        void Notify() {
            int n = pthread_cond_signal(&_cond);
            (void)n;
        }

        // 广播唤醒所有等待的线程
        void NotifyAll() {
            int n = pthread_cond_broadcast(&_cond);
            (void)n;
        }

        ~Cond() {
            int n = pthread_cond_destroy(&_cond);
            (void)n;
        }

    private:
        pthread_cond_t _cond; // 原生条件变量对象
    };

} // namespace CondModule