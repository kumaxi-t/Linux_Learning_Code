#include <iostream>
#include <signal.h>
#include <unistd.h>

void showPending(sigset_t *pending) {
  for (int i = 31; i >= 1; i--) {
    if (sigismember(pending, i))
      std::cout << "1";
    else
      std::cout << "0";
  }
  std::cout << std::endl;
}

int main() {
  sigset_t bset, obset, pending;
  sigemptyset(&bset);
  sigaddset(&bset, 2); // 把 2 号信号准备好

  // 1. 屏蔽 2 号信号
  sigprocmask(SIG_BLOCK, &bset, &obset);

  while (true) {
    // 2. 获取当前的 Pending 位图
    sigpending(&pending);
    // 3. 打印出来看
    showPending(&pending);
    sleep(1);
  }
  return 0;
}