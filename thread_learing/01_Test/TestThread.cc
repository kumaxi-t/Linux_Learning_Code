// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>
// void *run(void *arg) {
//   while (true) {
//     std::cout << "I am new a thread  " << std::endl;
//     sleep(1);
//   }
//   return nullptr;
// }

// int main() {
//   pthread_t tid;
//   int ret = pthread_create(&tid, nullptr, run, nullptr);
//   if (ret != 0) {
//     std::cerr << "Thread creation failed!" << std::endl;
//     return -1;
//   }
//   while (true) {
//     std::cout << "I am main thread" << std::endl;
//     sleep(1);
//   }
//   return 0;
// }

// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>

// void* threadRoutine(void* arg) {
//     // 线程一启动，立刻调用 pthread_self() 获取自身ID，并将自己设置为分离状态
//     pthread_detach(pthread_self());

//     std::cout << "I am a detached thread, working independently." <<
//     std::endl; sleep(2); std::cout << "Detached thread finished, resources
//     will be auto-reclaimed." << std::endl;

//     return nullptr;
// }

// int main() {
//     pthread_t tid;
//     if (pthread_create(&tid, nullptr, threadRoutine, nullptr) != 0) {
//         return -1;
//     }

//     // 主线程不需要，也不能再去调用 pthread_join(tid, nullptr) 了
//     // 主线程可以继续做自己的事情，比如沉睡3秒保证子线程能执行完
//     sleep(3);
//     std::cout << "Main thread exiting." << std::endl;
//     return 0;
// }

// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>

// void *testreturn(void *arg) {
//   std::cout << "【线程 1】正在运行，准备通过 return 退出..." << std::endl;

//   // 在堆上动态开辟一块空间，用于存放返回值
//   // 注意：绝对不能返回栈上的局部变量地址！
//   int *result = new int(100);

//   sleep(1);
//   return (void *)result; // 方式一：直接 return 返回值指针
// }
// void *testexit(void *arg) {
//   std::cout << "【线程 2】正在运行，准备通过 pthread_exit 退出..." << std::endl;

//   int *result = new int(200);

//   sleep(1);
//   pthread_exit((void *)result); // 方式二：调用 pthread_exit 终止自己

//   // 这里的代码永远不会被执行
//   std::cout << "【线程 2】这句打印不会触发！" << std::endl;
//   return nullptr;
// }
// void *testcancel(void *arg) {
//   while (true) {
//     std::cout << "【线程 3】正在死循环运行中..." << std::endl;
//     sleep(1);
//   }
//   return nullptr;
// }
// int main() {
//   pthread_t tid1, tid2, tid3;
//   void *retval = nullptr;
//   pthread_create(&tid1, nullptr, testreturn, nullptr);

//   pthread_join(tid1, &retval);
//   std::cout << "【主线程】成功回收【线程 1】，拿到返回值: " << *(int *)retval << std::endl;
//   delete (int *)retval;

//   sleep(1);
//   std::cout << "\n========== 演示 2：通过 pthread_exit 退出 ==========" << std::endl;
//   pthread_create(&tid2, nullptr, testexit, nullptr);
//   pthread_join(tid2, &retval);
//   std::cout << "【主线程】成功回收【线程 2】，拿到返回值: " << *(int *)retval << std::endl;
//   delete (int *)retval;

//   std::cout << "\n========== 演示 3：被 pthread_cancel 取消 ==========" << std::endl;
//   pthread_create(&tid3, nullptr, testcancel, nullptr);
//   sleep(2); // 让线程 3 先运行 2 秒

//   std::cout << "【主线程】发下命令，强行取消【线程 3】!" << std::endl;
//   pthread_cancel(tid3); // 主线程强行终止线程 3

//   pthread_join(tid3, &retval);
//   // 被 cancel 取消的线程，其退出码会被系统设置为常数 PTHREAD_CANCELED
//   if (retval == PTHREAD_CANCELED) {
//     std::cout << "【主线程】成功回收【线程 3】，该线程是被 cancel 异常终止的！(返回值等于 PTHREAD_CANCELED)" << std::endl;
//   }

//   std::cout << "\n所有线程演示完毕，主线程安全退出。" << std::endl;

//   return 0;
// }



// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>

// void* threadRoutine(void* arg) {
//     // 1. 线程自己把自身设置为分离状态
//     // pthread_self() 获取当前线程自身的 ID
//     pthread_detach(pthread_self());

//     std::cout << "【子线程】我已经将自己设置为分离状态（Detached）！" << std::endl;
//     for (int i = 0; i < 3; ++i) {
//         std::cout << "【子线程】独立工作中, count = " << i << std::endl;
//         sleep(1);
//     }
//     std::cout << "【子线程】工作完成，自动退出并由系统回收资源。" << std::endl;
//     return nullptr;
// }

// int main() {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, threadRoutine, nullptr);

//     // 2. 稍等片刻，确保子线程已经执行到了 pthread_detach
//     sleep(1);

//     std::cout << "【主线程】尝试调用 pthread_join 等待已被分离的子线程..." << std::endl;
//     int ret = pthread_join(tid, nullptr);

//     if (ret != 0) {
//         std::cout << "【主线程】pthread_join 失败！错误码 ret = " << ret 
//                   << " (说明分离后的线程确实不能被 join 了！)" << std::endl;
//     } else {
//         std::cout << "【主线程】pthread_join 成功！" << std::endl;
//     }

//     // 主线程继续干自己的活，保持程序不立刻退出
//     sleep(3);
//     std::cout << "【主线程】主线程安全退出。" << std::endl;
//     return 0;
// }


// #include <iostream>
// #include <pthread.h>
// #include <unistd.h>
// #include <cstdio>

// void* threadRoutine(void* arg) {
//     // 获取当前线程的 pthread_t ID
//     pthread_t tid = pthread_self();

//     // 打印格式说明：%p 专门用来以十六进制格式打印指针/内存地址
//     printf("【子线程】pthread_self() 返回的线程ID (十六进制): %p\n", (void*)tid);
    
//     // 在子线程栈区定义一个局部变量，打印它的内存地址
//     int thread_local_var = 10;
//     printf("【子线程】栈区局部变量 thread_local_var 的地址: %p\n", (void*)&thread_local_var);

//     while (true) {
//         sleep(1); // 保持运行，方便我们用 ps 命令在终端观察 LWP
//     }
//     return nullptr;
// }

// int main() {
//     pthread_t tid;
//     pthread_create(&tid, nullptr, threadRoutine, nullptr);

//     printf("【主线程】pthread_create() 第一个参数拿到的线程ID: %p\n", (void*)tid);

//     int main_local_var = 20;
//     printf("【主线程】栈区局部变量 main_local_var 的地址:     %p\n", (void*)&main_local_var);

//     printf("\n>>> 请打开另一个终端，输入命令观察真实 LWP 线程信息:\n");
//     printf("    ps -aL | grep pthread_id_lay\n\n");

//     while (true) {
//         sleep(1);
//     }
//     return 0;
// }



// #include <iostream>
// #include <string>
// #include <functional>
// #include <pthread.h>
// #include <unistd.h>

// class Thread {
// public:
//     // 定义外部传进来的业务逻辑函数类型 (例如 std::function<void()>)
//     using func_t = std::function<void()>;

//     // 构造函数：接收一个业务逻辑函数和线程名字
//     Thread(func_t func, const std::string& name = "Thread-Worker")
//         : _func(func), _name(name), _tid(0), _is_running(false) {}

//     // 启动线程
//     bool Start() {
//         if (_is_running) return true;

//         // TODO 1: 调用 pthread_create 创建线程
//         // 注意：第 3 个参数传什么？第 4 个参数传什么？
//         // int ret = pthread_create(/* ??? */);
//         int ret = pthread_create(&_tid, nullptr, Routine, this);
//         if (ret == 0) {
//             _is_running = true;
//             return true;
//         }
//         return false;
//     }       

//     // 回收线程
//     bool Join() {
//         if (!_is_running) return false;

//         // TODO 2: 调用 pthread_join 等待回收当前线程
//         // int ret = pthread_join(/* ??? */);
//         int ret = pthread_join(_tid, nullptr);
//         if (ret == 0) {
//             _is_running = false;
//             return true;
//         }
//         return false;
//     }
//     std::string GetName() const { return _name; }

//     ~Thread() {}

// private:
//     // 核心关键点：必须是 static！
//     static void* Routine(void* arg) {
//         // TODO 3: 1. 将 void* 类型的 arg 强转回当前 Thread 对象的指针 self
//         Thread* self = static_cast<Thread*> (arg);
//         // TODO 4: 2. 通过 self 指针，去执行对象内部真正保存的业务逻辑函数 _func()
//         self->_func();
//         return nullptr;
//     }

// private:
//     pthread_t _tid;
//     std::string _name;
//     bool _is_running;
//     func_t _func;
// };

// // ==================== 测试主函数 ====================

// // 这是用户定义的普通业务逻辑（打印数据）
// void MyTask() {
//     for (int i = 0; i < 3; ++i) {
//         std::cout << "【C++ 线程对象】正在执行业务任务, count = " << i << std::endl;
//         sleep(1);
//     }
// }

// int main() {
//     std::cout << "【主线程】开始测试 C++ 封装的 Thread 类..." << std::endl;

//     // 1. 实例化线程对象，把要干的活（MyTask）传进去
//     Thread t1(MyTask, "PrintThread");

//     // 2. 启动线程
//     t1.Start();
//     std::cout << "【主线程】成功启动子线程: " << t1.GetName() << std::endl;

//     // 3. 等待回收线程
//     t1.Join();
//     std::cout << "【主线程】子线程回收成功，程序结束。" << std::endl;

//     return 0;
// }




// 操作共享变量会有问题的售票系统代码
#include <iostream>
#include <unistd.h>
#include <pthread.h>

int ticket = 100; // 共享变量

void* route(void* arg) {
    const char* id = static_cast<const char*>(arg);
    while (true) {
        // 临界区开始
        if (ticket > 0) {
            usleep(1000); // 模拟漫长业务的过程
            std::cout << id << " sells ticket:" << ticket << std::endl;
            ticket--;
        } else {
            break;
        }
        // 临界区结束
    }
    return nullptr;
}

int main() {
    pthread_t t1, t2, t3, t4;
    // 创建4个线程，并发执行 route 函数
    pthread_create(&t1, nullptr, route, (void*)"thread 1");
    pthread_create(&t2, nullptr, route, (void*)"thread 2");
    pthread_create(&t3, nullptr, route, (void*)"thread 3");
    pthread_create(&t4, nullptr, route, (void*)"thread 4");

    // 主线程等待子线程结束
    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);
    pthread_join(t4, nullptr);
    return 0;
}




