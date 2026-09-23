#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
  int ticks = uptime();
  printf("Uptime before trace: %d\n", ticks);
  printf("Enabling tracing...\n");
  syscalltrace(1);

  getpid();
  uptime();
  sleep(1);

  int pid = fork();

  if (pid < 0){
  printf("Fork failed\n");
  exit(1);
  }
  if (pid == 0){
  printf("Child process checking syscalltrace status...\n");
  getpid();
  exit(0);
  }else{
  wait(0);
  syscalltrace(0);
  printf("Tracing disabled. \n");
  }
  exit(0);
}
