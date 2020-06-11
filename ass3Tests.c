#include "types.h"
#include "stat.h"
#include "user.h"

 
#define PGSIZE 4096
#define MAX_TOTAL_MEM 4096*32

int error = 0;

void
allocuvmTest(){
  printf(1,"STARTING allocuvmTest\n");
  int startSize = (int)sbrk(0);
  int size=0;
  sbrk(4*PGSIZE);
  size=(int)sbrk(0);
  if(startSize + 4*PGSIZE==size){
     printf(1,"allocuvmTest: process allocating mem succeed\n");
  }
  else{
     printf(1,"FAILED: allocuvmTest:  process allocating mem faild\n");
     error = 1;
  }
  sbrk(-4*PGSIZE);
  if( (int)sbrk(0) ==startSize){
    printf(1, "SUCCESS: allocuvmTest passed.\n");
  }else{
    printf(1, "FAILED: allocuvmTest failed allocating and deallocating. start size %d end size %d\n", startSize, (int)sbrk(0));
    error = 1;
  }
  
}

void
deallocuvmTest(){
  printf(1,"STARTING deallocuvmTest\n");
  int startSize = (int)sbrk(0);
  sbrk(PGSIZE);
  if(startSize + PGSIZE != (int)sbrk(0)){
    printf(1,"FAILED: deallocuvmTest: couldnt allocate page \n");
    error = 1;
  }

  sbrk(-1*PGSIZE);
  if(startSize != (int)sbrk(0)){
    printf(1,"FAILED: deallocuvmTest: couldnt deallocate page \n");
    error = 1;
  }
  else{
    printf(1, "SUCCESS: deallocuvmTest passed.\n");
  }
}

void
pageFaultTest(){
  printf(1,"STARTING pageFaultTest\n");
  int startSize = (int)sbrk(0);
  int size=0;
  size=(int)sbrk(MAX_TOTAL_MEM - startSize -PGSIZE);
  size=(int)sbrk(0);
  if(MAX_TOTAL_MEM-PGSIZE==size){
     printf(1,"pageFaultTest: process allocating mem succeed\n");
  }
  else{
     printf(1,"FAILED: pageFaultTest:  process allocating mem faild\n");
     error = 1;
  }
  printf(1,"pageFaultTest writing to all pages\n");
  for (int i = PGSIZE*4; i < MAX_TOTAL_MEM -PGSIZE; i = i + PGSIZE)
  {
    (*(char*)i)= 12;
  }
  size=(int)sbrk(-1*(MAX_TOTAL_MEM - startSize));
  printf(1, "SUCCESS: pageFaultTest passed.\n");
}

void
forkTest(){
  printf(1, "STARTING forkTest\n");
  char buf[PGSIZE*4];
  // printf(1,"forkTest: Start: free pages is: %d\n", getNumberOfFreePages());
  int pid = fork();
  if(pid < 0 ){
    printf(1, "forkTest FAIL: error couldnt fork \n");
  } 
  else {
    if(pid == 0){ 
      printf(1,"forkTest: child free pages is: %d\n", getNumberOfFreePages());
      sleep(100);
      buf[3]=2;
      buf[PGSIZE*4-1]=7;
      pid=buf[PGSIZE*4-1];
      printf(1,"forkTest: child free pages after changing buf is: %d\n", getNumberOfFreePages());
      exit();
    } 
    else{
      printf(1,"forkTest: parent free pages is: %d\n", getNumberOfFreePages());
      sleep(20);
      char buf2[PGSIZE*4];
      buf2[2]=7;
      buf2[PGSIZE*4-1]=7;
      buf[3]=buf2[PGSIZE*4-1];
      printf(1,"forkTest: parent free pages after buf2 is: %d\n", getNumberOfFreePages());
      wait();

    }
  }
  printf(1, "forkTest passed.\n");
}

void
partOneTest(){
  allocuvmTest();
  deallocuvmTest();
  pageFaultTest();
  if(error){
    printf(1, "FAILED tests part one.\n");
  }else{
    printf(1, "PASSED tests part one.\n");
  }
  error = 0;
}

void
partTwoTest(){
  forkTest();
} 


int main(int argc, char *argv[])
{
  printf(1, "ass3Tests start.\n");
 
  partOneTest();
  partTwoTest();
  printf(1, "ass3Tests passed.\n");
  exit();
}