#include "ProcessPool.hpp"

int main(){

  Processpool pp(gdefaultnum);
  pp.Create();
  pp.Debug();
  int task_code = 1;
  int cnt = 10;
  while(cnt--){
    pp.Run();
    sleep(1);
  }
  pp.Stop();
  sleep(1000);
  return 0;
}