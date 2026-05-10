#pragma once

#include <iostream>
#include <vector>
#include <ctime>
typedef void (*task_t)();
void Printlog(){
  std::cout << "Im a print log mission" << std::endl;
}
void Download(){
  std::cout << "Im a download mission" << std::endl;
}
void Upload(){
  std::cout << "Im a upload mission" << std::endl;
}


class TaskManager{
public:
  TaskManager(){
    srand(time(nullptr));
  }
  void Register(task_t t){
    _tasks.push_back(t);
  }
  int Code(){
    return rand() % _tasks.size();
  }
  void Execute(int code){
    if(code >= 0 && code < _tasks.size()){
      _tasks[code]();
    }
  }
  ~TaskManager(){}

private:
  std::vector<task_t> _tasks;

};