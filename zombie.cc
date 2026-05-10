#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    pid_t id = fork(); 

    if (id == 0) { 
        // 1. 子进程逻辑
        printf("我是子进程 [PID:%d]，我要活 5 秒...\n", getpid());
        sleep(5);
        printf("子进程 [PID:%d] 已经退出，变成了僵尸...\n", getpid());
        exit(10); // 子进程带着退出码 10 退出 
    } 
    else if (id > 0) {
        // 2. 父进程逻辑
        printf("我是父进程 [PID:%d]，我先不收尸，故意等 10 秒...\n", getpid());
        sleep(10); // 此时子进程已经退出了 5 秒，处于 Z 状态 

        printf("父进程醒了，准备收尸...\n");
        int status = 0;
        // 使用 waitpid 等待特定子进程 
        pid_t rid = waitpid(id, &status, 0); 

        if (rid > 0) {
            // 解析状态：查看是否正常退出
            if (WIFEXITED(status)) {
                printf("收尸成功！子进程退出码为: %d\n", WEXITSTATUS(status));
            }
        }
    }
    
    return 0;
}