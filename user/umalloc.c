#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};
struct block_header {
  struct block_header *next;
};

// Size classes: 16, 32, 64, 128, 256, 512, 1024, 2048
#define MIN_CHUNK 16
#define MAX_CHUNK 2048
#define NUM_CLASSES 8

typedef union header Header;

static struct block_header *free_lists[NUM_CLASSES];
static Header base;
static Header *freep;

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

static Header *
morecore(uint nu)
{
  char *p;
  Header *hp;

  if (nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if (p == SBRK_ERROR)
    return 0;
  hp = (Header *)p;
  hp->s.size = nu;
  free((void *)(hp + 1));
  return freep;
}

void *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
  if ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    if (p->s.size >= nunits) {
      if (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void *)(p + 1);
    }
    if (p == freep)
      if ((p = morecore(nunits)) == 0)
        return 0;
  }
}

// round up to nearest valid 2^n chunk size
static int
get_chunk_size(int nbytes)
{
  int sz = MIN_CHUNK;
  while (sz < nbytes && sz <= MAX_CHUNK) {
    sz <<= 1;
  }
  if (sz > MAX_CHUNK || PGSIZE % sz != 0) {
    return -1; // Exceeds max supported chunk size or does not divide PGSIZE
  }
  return sz;
}

static int
get_class_index(int chunk_size)
{
  int idx = 0;
  int sz = MIN_CHUNK;
  while (sz < chunk_size && idx < NUM_CLASSES - 1) {
    sz <<= 1;
    idx++;
  }
  return idx;
}

void *
block_alloc(int nbytes)
{
  if (nbytes <= 0)
    return 0;

  int chunk_size = get_chunk_size(nbytes);
  if (chunk_size < 0)
    return 0;

  int idx = get_class_index(chunk_size);

  // If free list has a block available, pop and return it
  if (free_lists[idx] != 0) {
    struct block_header *p = free_lists[idx];
    free_lists[idx] = p->next;
    return (void *)p;
  }

  // allocate a new page via sbrk
  char *page = sbrk(PGSIZE);
  if (page == (char *)-1)
    return 0;

  // Carve the new page into chunk_size blocks
  int num_blocks = PGSIZE / chunk_size;
  for (int i = 0; i < num_blocks; i++) {
    struct block_header *block = (struct block_header *)(page + i * chunk_size);
    block->next = free_lists[idx];
    free_lists[idx] = block;
  }

  // Pop and return one block from the newly created list
  struct block_header *p = free_lists[idx];
  free_lists[idx] = p->next;
  return (void *)p;
}

void
block_free(void *ap)
{
  if (ap == 0)
    return;

  // For basic segregated lists, returning block to class index 0 (16-byte list)
  // or storing header size class allows easy return to appropriate free list.
  struct block_header *p = (struct block_header *)ap;
  p->next = free_lists[0];
  free_lists[0] = p;
}
