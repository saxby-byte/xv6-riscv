#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
  int ticks = uptime();
  printf("Process Uptime: %d\n", uptime());
  printf("Testing syscalltrace() implementation...\n");
  syscalltrace(1);
  sbrk(0);
  getpid();
  uptime();
  sleep(1);
  
  int pid = fork();

  if (pid < 0){
  printf("Fork failed\n");
  exit(1);
  }
  if (pid == 0){
  printf("Child Tracking Value: %d\n", 0);
  uptime();
  exit(0);
  }else{
  printf("Parent Tracking Value: %d\n", 1);
    wait(0);
  }
  exit(0);
}
