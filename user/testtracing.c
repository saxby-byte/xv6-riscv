#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void){
  int ticks = uptime();
  printf("Process Uptime: %d\n", ticks);
  printf("Testing syscalltrace() implementation...\n");
  syscalltrace(1);
  sbrk(0);
  getpid();
  uptime();
  
  int pid = fork();

  if (pid < 0){
  printf("Fork failed\n");
  exit(1);
  }else{
    wait(0);
    syscalltrace(0);
  printf("Parent Tracking Value: %d\n", 1);
  }
  exit(0);
}
