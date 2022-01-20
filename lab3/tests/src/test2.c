#include "queue.h"
#include "thread.h"
#include <stdio.h>
#include <stdlib.h>


void T1(void* dummyArgument)
{
    printf("Thread %d\n", *(int*)dummyArgument);
}


int main()
{
  uthread_t tThreads[100];
  for(int i=0; i<100; i++){
    int* p = (int*)malloc(sizeof(int));
    *p = i;
    uthread_create(&tThreads[i],T1,p);
  }
 for(int i=0; i<100;i++){
  uthread_join(tThreads[i]);
 }
   return 0;
}
