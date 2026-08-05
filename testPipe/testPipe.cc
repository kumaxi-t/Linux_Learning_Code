#include <iostream>
#include <unistd.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void ChildWrite(int wfd){
  char buffer[1024];
  int cnt = 0;
  while(true){
    snprintf(buffer, sizeof(buffer), "child , pid: %d, cnt: %d", getpid(), cnt++);
    sleep(10);
    write(wfd, buffer, strlen(buffer));
  }
}
void FatherRead(int rfd){
  char buffer[1024];
  while(true){
    buffer[0] = 0;
    ssize_t n = read(rfd, buffer, sizeof(buffer) - 1);
    if(n > 0){
      buffer[n] = 0;
      std::cout << "child : " << buffer << std::endl;
    }

  }
}

int main(){
  //创建管道
  int fds[2] = {0};    // 0: read  1: write
  int n = pipe(fds);
  if(n < 0){
    std::cerr << "pipe error" << std::endl;
    return 1;
  }
  std::cout << "fds[0]: " << fds[0] << std::endl;
  std::cout << "fds[1]: " << fds[1] << std::endl;

  //创建子进程
  pid_t id = fork();
  if(id == 0){
    //son


    close(fds[0]);
    ChildWrite(fds[1]);


    close(fds[1]);
    exit(0);
  }
  //关闭不需要的读写端，形成通信信道
  close(fds[1]);
  FatherRead(fds[0]);
  waitpid(id, nullptr, 0);
  close(fds[0]);
  return 0;
}