#include "types.h"
#include "stat.h"
#include "user.h"


 
#define PGSIZE 4096
 
void read_mem(const char *mem) {
    printf(1, "read: mem[0]=%d , mem[1000]=%d\n", (int)mem[0], (int)mem[1000]);
}
 
void cow_test(void) {
    printf(1, "\n-- cow test --\n");
 
    printf(1, "malloc and set to zero...\n");
    char *mem = (char*)malloc(PGSIZE);
    for(int i = 0; i < PGSIZE; i++)
        mem[i] = 0;
 
    int pid = fork();
    if(pid == 0) { // child
        printf(1, "child (%d): writing 1 to allocated memory...\n", getpid());
        mem[0] = 0x1;
        mem[1000] = 0x1;
        sleep(200);
        printf(1, "child (%d): ", getpid());
        read_mem(mem);
        exit();
    }
    else {
        sleep(100);
        printf(1, "parent (%d): ", getpid());
        read_mem(mem);
        printf(1, "parent (%d): writing 2 to allocated memory...\n", getpid());
        mem[0] = 0x2;
        mem[1000] = 0x2;
        wait();
    }
}
 
void printfp(void) {
    // printf(1, "getfp: %d\n", getfp());
}
 
void free_pages_test(void) {
    printf(1, "\n-- free pages test --\n");
   
    const uint size = 25*PGSIZE + 1;
 
    printfp();
    printf(1, "allocating 26 pages...\n");
    sleep(300);
    char *mem = (char*)malloc(size);
    printfp();
    free(mem);
}
 
void page_fault_test(void) {
    printf(1, "\n-- page fault test --\n");
 
    const uint size = 25*PGSIZE + 1;
   
    printf(1, "allocating 26 pages...\n");
    char *mem = (char*)malloc(size);
    printf(1, "accessing last address. this should raise page fault");
    mem[size-1] = 0xFF;
    printf(1, "blablabla\n");
    free(mem);
}
 
 
int main(int argc, char *argv[])
{
  printf(1, "ass3Tests start.\n");
 
  //cow_test();
  //free_pages_test();
  page_fault_test();
 
  printf(1, "\nass3Tests end.\n");
  exit();
}