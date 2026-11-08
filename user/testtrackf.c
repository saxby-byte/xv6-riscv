#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_ALLOCS 300

int
main(int argc, char *argv[])
{
  void *ptrs[MAX_ALLOCS];
  int count = 0;

  printf("First Allocation:\n");
  ptrs[count] = block_alloc(28);
  count++;

  int init_frames = trackframes();
  printf("Process page table has %d frames.\n", init_frames);

  int curr_frames = init_frames;

  // Continue allocating until a new page frame is triggered
  while(curr_frames == init_frames && count < MAX_ALLOCS){
    ptrs[count] = block_alloc(28);
    count++;
    curr_frames = trackframes();
    if(curr_frames == init_frames){
      printf("Current page count: %d\n", curr_frames);
    } else {
      printf("Current page count: %d\n", curr_frames);
    }
  }

  printf("Frames allocated: %d\n", count);
  printf("================================================= Beginning block_free\n");
  printf("============================================\n");

  for(int i = 0; i < count; i++){
    block_free(ptrs[i]);
  }

  printf("All blocks freed.\n");
  exit(0);
}
