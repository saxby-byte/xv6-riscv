#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
  int ticks = uptime();
  printf("Uptime before trace: %d\n", ticks);
  printk("Enabling tracing...\n");
  syscalltrace(1);

  getpid();
  uptime();
  sleep(1);

  int pid = fork();

  if (pid < 0){
  printk("Fork failed\n");
  exit(1);
  }
  if (pid == 0){
  printk("Child process checking syscalltrace status...\n");
  getpid();
  exit(0);
  }else{
  wait(0);
  syscalltrace(0);
  printk("Tracing disabled. \n");
  }
  exit(0);
}
