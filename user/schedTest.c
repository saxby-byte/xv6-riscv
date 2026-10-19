#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Helper function to check if a number is an Armstrong number
void compute_armstrong(int limit) {
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
}

int main(int argc, char *argv[]) {
  int limit = 1000000;
  if (argc > 1) {
    limit = atoi(argv[1]);
    if (limit < 1000000) {
      limit = 1000000; // Minimum limit requirement
    }
  }

  int priorities[8] = {0, 0, 5, 5, 10, 10, 15, 15};

  // Enable debug mode
  debug(1);

  for (int i = 0; i < 8; i++) {
    int pid = kpriority_fork(priorities[i]);
    if (pid < 0) {
      printf("Error: priority_fork failed\n");
      exit(1);
    } else if (pid == 0) {
      // Child process
      compute_armstrong(limit);
      exit(0);
    }
  }

  // Parent process waits for all 8 children to finish
  for (int i = 0; i < 8; i++) {
    wait(0);
  }

  // Reset debug mode to run mode
  debug(0);

  exit(0);
}
