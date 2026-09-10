#pragma once

#include <pthread.h>
#include <iostream>


namespace MutexModule {

class Mutex {
public:
    Mutex() {
        pthread_mutex_init(&_mutex, nullptr);
    }

    // 禁止拷贝和赋值
    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;

    void Lock() {
        int n = pthread_mutex_lock(&_mutex);
        (void)n;
    }

    void Unlock() {
        int n = pthread_mutex_unlock(&_mutex);
        (void)n;
    }

    // 增加获取原生 pthread_mutex_t 指针的接口，方便 pthread_cond_wait 使用
    pthread_mutex_t* GetMutexOriginal() {
        return &_mutex;
    }

    ~Mutex() {
        pthread_mutex_destroy(&_mutex);
    }

private:
    pthread_mutex_t _mutex;
};

// RAII 风格的锁守护者
class LockGuard {
public:
    explicit LockGuard(Mutex& mutex) : _mutex(mutex) {
        _mutex.Lock();
    }

    ~LockGuard() {
        _mutex.Unlock();
    }

    // 禁止拷贝和赋值
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    Mutex& _mutex;
};

} // namespace MutexModule