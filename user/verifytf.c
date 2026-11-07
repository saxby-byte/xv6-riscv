#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int initial_frames = trackframes();
  printf("Initial allocated frames: %d\n", initial_frames);

  // Allocate 28 bytes (rounds up to 32 bytes)
  void *ptr1 = block_alloc(28);
  int frames_after_first = trackframes();
  printf("Frames after allocating 28 bytes: %d\n", frames_after_first);

  // Keep allocating 28 bytes until page frame count increases
  int count = 1;
  void *ptrs[200];
  ptrs[0] = ptr1;

  while (1) {
    int current_frames = trackframes();
    if (current_frames > frames_after_first) {
      printf("New page allocated! Total allocations: %d, New frame count: %d\n", 
             count, current_frames);
      break;
    }
    ptrs[count] = block_alloc(28);
    count++;
  }

  // Clean up memory
  for (int i = 0; i < count; i++) {
    block_free(ptrs[i]);
  }

  printf("Test completed successfully.\n");
  exit(0);
}
