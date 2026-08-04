#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "Lock.hpp" 

using namespace MutexModule;

int ticket = 50;
Mutex global_mutex; 

void* route(void* arg) {
    char* name = static_cast<char*>(arg);
    
    while(true) {
        // 利用 C++ 作用域花括号 {} 来限定锁守卫的生命周期
        {
            LockGuard lockguard(global_mutex); // 自动加锁

            if(ticket > 0) {
                usleep(1000);
                printf("%s 成功卖出，剩余票数: %d\n", name, ticket);
                ticket--;
            } else {
                break; // 即使在这里直接 break 退出，局部变量 lockguard 也会自动析构并自动解锁
            }
        } // 离开花括号，lockguard 变量生命周期结束，自动调用 ~LockGuard() 解锁
        usleep(1000);
    }
    return nullptr;
}

int main() {
    pthread_t t1, t2, t3;
    pthread_create(&t1, nullptr, route, (void*)"窗口 A");
    pthread_create(&t2, nullptr, route, (void*)"窗口 B");
    pthread_create(&t3, nullptr, route, (void*)"窗口 C");

    pthread_join(t1, nullptr);
    pthread_join(t2, nullptr);
    pthread_join(t3, nullptr);

    printf("所有票据售罄，安全退出。\n");
    return 0;
}

