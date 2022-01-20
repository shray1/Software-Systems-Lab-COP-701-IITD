#include "queue.h"
#include "thread.h"
#include <stdio.h>
#include <stdlib.h>

Lock* gLock;
void T1(void* dummyArgument)
{
    printf("Thread 1\n");
}
void T2(void* dummyArgument)
{
    printf("Thread 2\n");
    lAcquire(gLock);
    uthread_yeild();
    printf("Thread 2 After Yeild()\n");
    lRelease(gLock);
}
void T3(void* dummyArgument)
{
    printf("Thread 3 before lock.\n");
    lAcquire(gLock);
    printf("Thread 3\n");
    lRelease(gLock);
}

int main()
{
    gLock = lInit();
   uthread_t tThreads[3];
    uthread_create(&(tThreads[0]), T1, NULL);
    uthread_create(&(tThreads[1]), T2, NULL);
    uthread_create(&(tThreads[2]), T3, NULL);
   for(int i=0; i<3;i++){
    uthread_join(tThreads[i]);
   }
   return 0;
}
