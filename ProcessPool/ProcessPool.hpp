#ifndef __PROCESS_POOL_HPP__
#define __PROCESS_POOL_HPP__
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <sys/wait.h>
#include "task.hpp"
class Channel{
public:
  Channel(int fd, pid_t id):_wfd(fd), _subid(id){
    _name =  "channel-" + std::to_string(_wfd) + "-" + std::to_string(_subid);
  }
  void Send(int code){
    int n = write(_wfd, &code, sizeof(code));
    (void)n;
  }
  void Close(){
    close(_wfd);
  }
  void Wait(){
    pid_t rid = waitpid(_subid, nullptr, 0);
    (void) rid;
  }
  int fd(){ return _wfd;}
  pid_t SubId(){  return _subid;}
  std::string Name(){ return _name;}
  ~Channel(){}
private:
  int _wfd;
  pid_t _subid;
  std::string _name;

};

class ChannelManger{
public:
  ChannelManger():_next(0){}
  void Insert(int wfd, pid_t subid){
    _channels.emplace_back(wfd, subid);
  }
  void PrintChannel(){
    for(auto &channel : _channels){
      std::cout << channel.Name() << std::endl;
    }
  }
  Channel &Slect(){
    auto &c = _channels[_next];
    _next++;
    _next %= _channels.size();
    return c;
  }
  void StopSubProcess(){
    for(auto &channel : _channels){
      channel.Close();
      std::cout << "close: " << channel.Name() << std::endl;
    }
  }
  void WaitSubProcess(){
    for(auto &channel : _channels){
      channel.Wait();
      std::cout << "recycle: " << channel.Name() << std::endl;
    }
  }
  ~ChannelManger(){}
private:
  std::vector<Channel> _channels;
  int _next;
};

const int gdefaultnum = 5;
class Processpool{
public:
  Processpool(int num):_process_num(num){
    _tm.Register(Printlog);
    _tm.Register(Download);
    _tm.Register(Upload);
  }
  void Work(int rfd){
    while(true){
      int code = 0;
      ssize_t n = read(rfd, &code, sizeof(code));
      if(n > 0){
        if(n != sizeof(code)) continue;
        std::cout << "子进程 " << getpid() << "收到一个任务码：" << code << std::endl;
        _tm.Execute(code);
      }else if (n == 0){
        std::cout << "子进程退出" << std::endl;
        break;
      }else{
        std::cout << "读取错误" << std::endl;
        break;
      }
      //std::cout << "Im child, my rfd is :" << rfd << std::endl;
      //sleep(1);
    }
  }
  bool Create(){
    for(int i = 0; i < _process_num; i++){
      int pipefd[2] = {0};
      int n = pipe(pipefd);
      if(n < 0) return false;

      pid_t subid = fork();
      if(subid < 0) return false;
      else if (subid == 0){
        //子进程
        close(pipefd[1]);
        Work(pipefd[0]);

        close(pipefd[0]);
        exit(0);
      }else{
        //父进程
        close(pipefd[0]);
        _cm.Insert(pipefd[1], subid);
      }
    }
    return true;
  }
  void Debug(){
    _cm.PrintChannel();
  }
  void Run(){
    int taskcode = _tm.Code();
    auto &c = _cm.Slect();//选择一个信道
    std::cout << "选择了一个子进程： " << c.Name() << std::endl;
    c.Send(taskcode);//发送任务
    std::cout << "发送了一个子进程： " << taskcode << std::endl;
  }
  void Stop(){
    _cm.StopSubProcess();
    _cm.WaitSubProcess();
  }
  ~Processpool(){}
private:
  ChannelManger _cm;
  int _process_num;
  TaskManager _tm;
};


#endif