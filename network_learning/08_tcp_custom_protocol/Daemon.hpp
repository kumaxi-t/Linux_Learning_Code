#pragma once
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace DaemonModule
{
  const char *default_root = "/";
  const char *dev_null = "/dev/null";

  // ischdir: 是否切换工作目录到根目录
  // isclose: 是否彻底关闭或重定向 0, 1, 2
  void Daemon(bool ischdir = false, bool isclose = true)
  {
    // 忽略可能引起程序异常退出的信号
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    // 让自己绝对不要成为组长进程
    if (fork() > 0)
    {
      exit(0); // 父进程是组长，直接退出
    }

    // 此时只有子进程能走到这里，调用 setsid 自立门户建立新会话
    setsid();

    // 更改当前进程的工作目录
    if (ischdir)
    {
      chdir(default_root);
    }

    // 重设文件掩码
    umask(0);

    // 已经变成守护进程，不再与终端输入输出关联，重定向到黑洞
    if (isclose)
    {
      int fd = open(dev_null, O_RDWR);
      if (fd >= 0)
      {
        dup2(fd, 0); // 标准输入 -> /dev/null
        dup2(fd, 1); // 标准输出 -> /dev/null
        dup2(fd, 2); // 标准错误 -> /dev/null
        close(fd);   // 复制完成后关闭原 fd
      }
    }
  }
}