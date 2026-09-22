#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void){
  int ticks = uptime();
  printk("Uptime before trace: %d\n", ticks);
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
