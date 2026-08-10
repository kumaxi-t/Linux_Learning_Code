#ifndef _THREAD_H_
#define _THREAD_H_

#include <iostream>
#include <pthread.h>
#include <string>
#include <utility>
#include <cstring>
#include <functional>
#include <atomic>

namespace ThreadModule {

    inline std::atomic<std::uint32_t> g_thread_cnt{1};

    template<typename T>
    class Thread {
    public:
        using func_t = std::function<void(T)>;

    public:
        Thread(func_t func, T data, const std::string &name = "")  
            : _tid(0),
              _isdetach(false), 
              _isrunning(false),
              _res(nullptr),
              _func(func),
              _data(data) {
            if (name.empty()) {
                _name = "Thread-" + std::to_string(g_thread_cnt++);
            } else {
                _name = name;
            }
        }

        Thread(const Thread&) = delete;
        Thread& operator=(const Thread&) = delete;

        Thread(Thread&& other) noexcept
            : _tid(other._tid),
              _name(std::move(other._name)),
              _isdetach(other._isdetach),
              _isrunning(other._isrunning),
              _res(other._res),
              _func(std::move(other._func)),
              _data(std::move(other._data)) {
            other._tid = 0;
            other._isrunning = false;
        }


        static void *Routine(void *arg) {
            Thread *self = static_cast<Thread*>(arg);
            
            // 如果被设置了分离属性，自动分离自身
            if (self->_isdetach) {
                pthread_detach(self->_tid);
            }

            // 执行真正绑定的回调函数
            if (self->_func) {
                self->_func(self->_data);
            }

            self->_isrunning = false;
            return nullptr;
        }

        // 启动线程
        bool Start() {
            if (_isrunning) return true;

            int n = pthread_create(&_tid, nullptr, Routine, this);
            if (n != 0) { 
                std::cerr << "[" << _name << "] Create thread error: " << strerror(n) << std::endl;
                return false;
            }

            _isrunning = true;
            return true;
        }

        // 分离线程
        void Detach() {
            if (_isdetach) return;
            if (_isrunning) {
                pthread_detach(_tid);
            }
            _isdetach = true;
        }

        // 回收线程
        void Join() {
            if (_isdetach || !_isrunning) return;

            int n = pthread_join(_tid, &_res);
            if (n != 0) {
                std::cerr << "[" << _name << "] Join thread error: " << strerror(n) << std::endl;
            } else {
                _isrunning = false;
            }
        }

        // 强制取消线程
        bool Stop() {
            if (_isrunning) {
                int n = pthread_cancel(_tid);
                if (n != 0) {
                    std::cerr << "[" << _name << "] Stop thread error: " << strerror(n) << std::endl;
                    return false;
                }
                _isrunning = false;
                return true;
            }
            return false;
        }

        // 常用 Getter 接口
        std::string Name() const { return _name; }
        bool IsRunning() const { return _isrunning; }
        pthread_t GetTid() const { return _tid; }

        ~Thread() {
            if (_isrunning && !_isdetach) {
                Join();
            }
        }

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