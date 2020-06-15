#include "types.h"
#include "stat.h"
#include "user.h"

 
#define PGSIZE 4096
#define MAX_TOTAL_MEM 4096*32
#define NFUA        0
#define LAPA        1
#define SCFIFO      2
#define AQ          3
#define NONE        4


int error = 0;

void
allocuvmTest(){
  printf(1,"-----------STARTING allocuvmTest-----------\n");
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
    printf(1, "-----------SUCCESS: allocuvmTest passed.-----------\n");
  }else{
    printf(1, "FAILED: allocuvmTest failed allocating and deallocating. start size %d end size %d\n", startSize, (int)sbrk(0));
    error = 1;
  }
  
}

void
deallocuvmTest(){
  printf(1,"-----------STARTING deallocuvmTest-----------\n");
  int startSize = (int)sbrk(0);
  sbrk(PGSIZE);
  if(startSize + PGSIZE != (int)sbrk(0)){
    printf(1,"FAILED: deallocuvmTest: couldnt allocate page \n");
    error = 1;
  }else{
    printf(1,"deallocuvmTest: allocated new page successfully\n");
  }

  sbrk(-1*PGSIZE);
  if(startSize != (int)sbrk(0)){
    printf(1,"FAILED: deallocuvmTest: couldnt deallocate page \n");
    error = 1;
  }
  else{
    printf(1, "-----------SUCCESS: deallocuvmTest passed.-----------\n");
  }
}

void
pageFaultTest(){
  printf(1,"-----------STARTING pageFaultTest-----------\n");
  int startSize = (int)sbrk(0);
  int size=0;
  size=(int)sbrk(MAX_TOTAL_MEM - startSize -PGSIZE);
  size=(int)sbrk(0);
  if(MAX_TOTAL_MEM-PGSIZE==size){
     printf(1,"SUCCESS: process allocating mem succeed\n");
  }
  else{
     printf(1,"FAILED: pageFaultTest:  process allocating mem faild\n");
     error = 1;
  }
  printf(1,"pageFaultTest: writing to all pages\n");
  for (int i = PGSIZE*4; i < MAX_TOTAL_MEM - 10 * PGSIZE; i = i + PGSIZE)
  {
    (*(char*)i)= 12;
  }
  printf(1,"pageFaultTest: succeed writing to all pages\n");
  size=(int)sbrk(-1*(MAX_TOTAL_MEM - startSize -PGSIZE));
  printf(1, "-----------SUCCESS: pageFaultTest passed-----------\n");
}

void
forkTest(){
  int freePgs;
  printf(1, "-----------STARTING forkTest-----------\n");
  sbrk(4*PGSIZE);
  int pid = fork();
  if(pid < 0 ){
    printf(1, "FAILED: forktest: error couldnt fork \n");
  } 
  else {
    if(pid == 0){ 
      freePgs = getNumberOfFreePages();
      (*(char*)(PGSIZE*6))=7;
      if(freePgs-1 != getNumberOfFreePages()){
        printf(1, "FAILED: forktest: incorrect num of pages. \n");
      }else{
        printf(1, "SUCCESS: number of free pages decreased after writing to shared page. \n");
      }
      exit();
    } 
    else{
      sleep(50);
      freePgs = getNumberOfFreePages();      
      (*(char*)(PGSIZE*6))=8;
      if(freePgs != getNumberOfFreePages()){
        printf(1, "FAILED: forktest: incorrect num of pages- shouldn't allocate new page because its already duplicated. \n");
      }else{
        printf(1, "SUCCESS: number of free pages stays the same. Didnt allocate new page because its already duplicated. \n");
      }
      (*(char*)(PGSIZE*7))=8;
      if(freePgs-1 != getNumberOfFreePages()){
        printf(1, "FAILED: forktest: incorrect num of pages- shoulded allocate new page because it didnt duplicated yet. \n");
      }else{
        printf(1, "SUCCESS: number of free pages decreased after writing to shared page(other than the first that has been written by child). \n");
      }
      wait();
    }
  }
  printf(1, "-----------forkTest passed.-----------\n");
}

void
partOneTest(){
  allocuvmTest();
  deallocuvmTest();
  pageFaultTest();
  if(error){
    printf(1, "----------------------------FAILED tests part one----------------------------\n");
  }else{
    printf(1, "----------------------------PASSED tests part one----------------------------\n");
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

  #if SELECTION==NFUA
    printf(1,"------------POLICY: NFUA---------------\n");
  #endif
  #if SELECTION==LAPA
    printf(1,"------------POLICY: LAPA---------------\n");
  #endif
  #if SELECTION==SCFIFO
    printf(1,"------------POLICY: SCFIFO---------------\n");
  #endif
  #if SELECTION==AQ
    printf(1,"------------POLICY: AQ---------------\n");
  #endif
  #if SELECTION==NONE
    printf(1,"------------POLICY: NONE---------------\n");
  #endif

  partOneTest();
  partTwoTest();
  printf(1, "ass3Tests passed.\n");
  exit();
}