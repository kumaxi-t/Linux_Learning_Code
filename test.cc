#include <iostream>
int main(int argc, char* argv[], char* envp[]){
  for(int i = 0; envp[i] != nullptr; i++){
    std::cout << "envp[" << i << "]: "<< envp[i] << std::endl; 
  }
  return 0;
}