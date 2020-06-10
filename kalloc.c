// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
  uint cow_refs[PHYSTOP/PGSIZE];
  struct spinlock cowLock;
  int use_cow_lock;
} kmem;

uint
get_cow_page_idx(uint va)
{
  uint pa = V2P(va);
  return pa/PGSIZE;  
}

void init_cow_refs(uint va){
  kmem.cow_refs[get_cow_page_idx(va)] =1 ;
}

//returns counter after the operation
int
increment_cow_refs(uint va){
  uint idx=get_cow_page_idx(va);
  int refs = kmem.cow_refs[idx]++;
  return refs;
}

int
decrement_cow_refs(uint va){
  uint idx=get_cow_page_idx(va);
  kmem.cow_refs[idx]--;
  return kmem.cow_refs[idx];
}

int getCowRefs(uint va){
  return kmem.cow_refs[get_cow_page_idx(va)];
}
// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  initlock(&kmem.cowLock, "kmemCowLock");
  kmem.use_lock = 0;
  kmem.use_cow_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
  kmem.use_cow_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE){
    kmem.cow_refs[get_cow_page_idx((uint)p)] = 0;
    kfree(p);
  }
}
void
acquireCowLock(){
  if(kmem.use_cow_lock)
    acquire(&kmem.cowLock);
} 

void 
releaseCowLock(){
  if(kmem.use_cow_lock)
    release(&kmem.cowLock);
} 
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");
  acquireCowLock();
  if(decrement_cow_refs((uint)v) <= 0){
    memset(v, 1, PGSIZE);

    if(kmem.use_lock)
      acquire(&kmem.lock);
    r = (struct run*)v;
    r->next = kmem.freelist;
    kmem.freelist = r;
    if(kmem.use_lock)
      release(&kmem.lock);
  }
  releaseCowLock();  
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r){
    kmem.freelist = r->next;
    acquireCowLock();
    init_cow_refs((uint)r);
    releaseCowLock();
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

int getNumberOfFreePages(){
  struct run *r = kmem.freelist;
  int count = 0;
  while(r) { 
    r = r->next;
    count++;
  }
  return count;
}
