#include "Thread.hpp"
#include <unistd.h>

using namespace ThreadModule;

int main(){
  Thread<int> t([](int){
    while(true){
      std::cout << "makabaka" << std::endl;
      sleep(1);
    }
  }, 0);
  t.Start();

  sleep(5);
  t.Stop();

  sleep(5);
  t.Join();
  

  return 0;
}