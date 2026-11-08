#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.
#define PGSIZE 4096

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};
struct block_node {
  struct block_node *next;
};

// Size classes: 32, 64, 128, 256, 512, 1024, 2048
static struct block_node *freelists[7];

typedef union header Header;

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

static void
print_hex_64(uint64 val)
{
  char buf[17];
  char hex_digits[] = "0123456789ABCDEF";
  buf[16] = '\0';
  for(int i = 15; i >= 0; i--){
    buf[i] = hex_digits[val & 0xF];
    val >>= 4;
  }
  printf("0x%s", buf);
}

static int
get_chunk_size(int nbytes)
{
  int sz = 32;
  while(sz < nbytes && sz <= 2048){
    sz <<= 1;
  }
  if(nbytes > 2048) return -1;
  return sz;
}

static int
get_list_index(int chunk_size)
{
  int idx = 0;
  int sz = 32;
  while(sz < chunk_size){
    sz <<= 1;
    idx++;
  }
  return idx;
}

void*
block_alloc(int nbytes)
{
  if(nbytes <= 0) return 0;

  int chunk_sz = get_chunk_size(nbytes);
  if(chunk_sz < 0) return 0;

  int idx = get_list_index(chunk_sz);

  // If freelist is empty, request a new page via sbrk
  if(freelists[idx] == 0){
    char *page = sbrk(PGSIZE);
    if(page == (char*)-1) return 0;

    int num_chunks = PGSIZE / chunk_sz;
    // Link from top of page down (0xFE0 down to 0x000)
    for(int i = 0; i < num_chunks; i++){
      struct block_node *node = (struct block_node*)(page + i * chunk_sz);
      node->next = freelists[idx];
      freelists[idx] = node;
    }
  }

  struct block_node *alloc_node = freelists[idx];
  freelists[idx] = alloc_node->next;

  uint64 addr = (uint64)alloc_node;
  uint64 page_base = addr & ~0xFFFL;

  printf("Allocated %d-byte chunk at ", chunk_sz);
  print_hex_64(addr);
  printf(" (page ");
  print_hex_64(page_base);
  printf(")\n");

  return (void*)alloc_node;
}

void
block_free(void *ap)
{
  if(ap == 0) return;

  uint64 addr = (uint64)ap;
  uint64 page_base = addr & ~0xFFFL;

  printf("Freed 32-byte chunk at ");
  print_hex_64(addr);
  printf(" (page ");
  print_hex_64(page_base);
  printf(")\n");

  struct block_node *node = (struct block_node*)ap;
  int idx = get_list_index(32);
  node->next = freelists[idx];
  freelists[idx] = node;
}
