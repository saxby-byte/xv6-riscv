#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Helper function to check if a number is an Armstrong number
int compute_armstrong(int limit) {
  int count = 0;
  for (int i = 2; i <= limit; i++) {
    int temp = i;
    int digits = 0;

    // Count digits
    while (temp != 0) {
      digits++;
      temp /= 10;
    }

    temp = i;
    int sum = 0;
    while (temp != 0) {
      int rem = temp % 10;
      int pow = 1;
      for (int d = 0; d < digits; d++) {
        pow *= rem;
      }
      sum += pow;
      temp /= 10;
    }

    if (sum == i) {
      count++;
    }
  }
  return count;
}

int main(int argc, char *argv[]) {
  int debug_val = 0;
  int limit = 1000000;

  printf("Enter a Debug Value (Must be either 0 or 1):\n");

  char buf[16];
  gets(buf, sizeof(buf));
  debug_val = atoi(buf);

  debug(debug_val);

  printf("\n");
 
  int priorities[8] = {0, 0, 5, 5, 10, 10, 15, 15};
  int pids[8];
  
  for (int i = 0; i < 8; i++) {
    int pid = priority_fork(priorities[i]);
    if (pid < 0) {
      printf("Error: priority_fork failed\n");
      exit(1);
    } else if (pid == 0) {
      // Child process
      int start_tick = uptime();
      int current_pid = getpid();
      
      int total_armstrong = compute_armstrong(limit);
      int end_tick = uptime();
      int duration = end_tick - start_tick;

      if (debug_val == 0) {
        printf("Process %d:\n", current_pid);
        printf("Number of Armstrong Numbers between 2 and %d: %d\n", limit, total_armstrong);
        printf("Time for process to complete:%dticks\n\n", duration);
      }

      exit(0);
    } else {
      pids=[i];
    }
  }
debug(debug_val);
  if (debug_val == 1) {
    for (int i = 0; i < 8; i++) {
      printf("Forking a process with PID: %d and Priority: %d\n\n", pids[i], priorities[i]);
    }
  }
  // Wait for all 8 child processes
  for (int i = 0; i < 8; i++) {
    wait(0);
  }

  // Reset debug mode to run mode
  debug(0);

  exit(0);
}
