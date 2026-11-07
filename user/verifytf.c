#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_ALLOCS 200

int
main(int argc, char *argv[])
{
  void *allocated_ptrs[MAX_ALLOCS];
  int alloc_count = 0;

  int initial_frames = trackframes();
  printf("Initial frames allocated: %d\n", initial_frames);

  int current_frames = initial_frames;
  
  // Keep allocating 28 bytes until dynamic allocation triggers a new page frame
  while(current_frames == initial_frames && alloc_count < MAX_ALLOCS){
    void *p = block_alloc(28);
    if(p == 0){
      printf("Allocation failed!\n");
      exit(1);
    }
    allocated_ptrs[alloc_count++] = p;
    current_frames = trackframes();
    printf("Allocated 28 bytes (alloc #%d). Current frames: %d\n", alloc_count, current_frames);
  }

  if(current_frames > initial_frames){
    printf("Frame count increased from %d to %d. Freeing allocated blocks...\n", initial_frames, current_frames);
    for(int i = 0; i < alloc_count; i++){
      block_free(allocated_ptrs[i]);
    }
    printf("Memory freed successfully. Terminating.\n");
  } else {
    printf("Max allocations reached without frame count increase.\n");
  }

  exit(0);
}
