#include "types.h"
#include "stat.h"
#include "user.h"

 
#define PGSIZE 4096
#define MAX_TOTAL_MEM 4096*32

void
allocuvmTest(){
  printf(1,"starting allocuvmTest\n");
  int startSize = (int)sbrk(0);
  int size;
  printf(1,"allocuvmTest: procces start size is %d\n",startSize);
  size=(int)sbrk(MAX_TOTAL_MEM - startSize);
  size=(int)sbrk(0);
  printf(1,"allocuvmTest: procces mem size is %d\n",size);
  if((int)sbrk(PGSIZE) != -1)
    printf(1,"allocuvmTest: prob in allocuvm\n");
  size=(int)sbrk(-1*(MAX_TOTAL_MEM - startSize));
  printf(1, "allocuvmTest passed.\n");
}
 void
deallocuvmTest(){
  printf(1,"starting deallocuvmTest\n");
  int startSize = (int)sbrk(0);
  int size;
  printf(1,"deallocuvmTest: procces start size is %d\n",startSize);
  size=(int)sbrk(MAX_TOTAL_MEM - startSize);
  printf(1,"deallocuvmTest: procces mem is %d\n",size);
  size=(int)sbrk(-1*PGSIZE);
  printf(1,"deallocuvmTest: procces mem is %d\n",size);
  size=(int)sbrk(-1*((MAX_TOTAL_MEM-PGSIZE) - startSize));
  printf(1, "deallocuvmTest passed.\n");
}

void
pageFaultTest(){
  printf(1,"starting pageFaultTest\n");
  int size;
  int startSize = (int)sbrk(0);
  size=(int)sbrk(MAX_TOTAL_MEM - startSize);
  if(size == -1)
    printf(1,"pageFaultTest: prob in allocuvm\n");
  printf(1,"pageFaultTest writing to all pages\n");
  for (int i = PGSIZE*4; i < MAX_TOTAL_MEM; i = i + PGSIZE)
  {
    (*(char*)i)= 12;
  }
  size=(int)sbrk(-1*(MAX_TOTAL_MEM - startSize));
  printf(1, "allocuvmTest passed.\n");
}

void
forkTest(){
  printf(1, "forkTest: after fork \n");
  char buf[PGSIZE*4];
  int pid = fork();
  if(pid < 0 ){
    printf(1, "forkTest: error couldnt fork \n");
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
Test_2(){
   int num;
   int pid = fork();
   if(pid < 0 ){
      printf(1, "Fork: error \n");
      exit();
   } 
   // child
   if(pid == 0){ 
     num = 1;
     printf(1, "Child [suppose to be 1]: num is %d\n", num);
     exit();
   } 
   // parent
   else{
     num = 0;
     sleep(10);
     printf(1, "Parent [suppose to be 0]: num is %d\n", num);
     wait();
   }
}
void
partOneTest(){
  // allocuvmTest();
  // deallocuvmTest();
  // pageFaultTest();
} 
void
partTwoTest(){
  forkTest();
  // Test_2();
} 
int main(int argc, char *argv[])
{
  printf(1, "ass3Tests start.\n");
 
  partOneTest();
  partTwoTest();
  printf(1, "ass3Tests passed.\n");
  exit();
}