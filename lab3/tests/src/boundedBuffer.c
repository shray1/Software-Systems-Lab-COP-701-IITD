#include "queue.h"
#include "thread.h"
#include <stdio.h>
#include <stdlib.h>

#define N 5  //Buffer Size.
#define M 5  //Producer,Consumer thread counts.
Semaphore*  gSemFull;
Semaphore*  gSemEmpty;
Lock* gMutexQueue;
//int  gPCCounter=0; // ProducerConsumerCounter.
int gProducedId = 0;
Queue* gContainerQueue;
 
void Producer(void* dummyArgument)
{
    int limit = 10*N;
    int pCounter = 0;
    while(pCounter < limit)
    {
      int* pProducedItem = (int*)malloc(sizeof(int));
      *pProducedItem = gProducedId++;
      semP(gSemEmpty);
      lAcquire(gMutexQueue);
      printf("Producing Item = %d , by ProducerID = %d\n", *pProducedItem, uthread_getId());
      qPush(gContainerQueue,pProducedItem);
      //gPCCounter++;
      lRelease(gMutexQueue);
      semV(gSemFull);
      for(int i=0; i<1000000; i++){
        i=i+1;
      }
      pCounter++;
    } 
}

void Consumer(void* dummyArgument)
{
    int iConsumedItem;
    int limit = 10*N;
    int cCounter = 0;
    while(cCounter < limit)
    {
     semP(gSemFull);
     lAcquire(gMutexQueue);
     int* pConsumedItem = (int*)qPop(gContainerQueue);
     printf("Item Consumed = %d , by ConsumerID = %d\n",*pConsumedItem,uthread_getId());
     //gPCCounter++;
     lRelease(gMutexQueue);
     semV(gSemEmpty);
     free(pConsumedItem);
     for(int i=0; i<1000000; i++){;}
     cCounter++;
   }
}


int main()
{
   //gPCCounter = 0;
   gContainerQueue = qInit();
   gMutexQueue = lInit();
   gSemEmpty = semInit(N);
   gSemFull = semInit(0);
   gSemFull->iMaxValuePossilbe = N;
   uthread_t tProducers[M];
   uthread_t tConsumers[M];
   for(int i=0; i<M;i++){
    uthread_create(&(tProducers[i]), Producer, NULL);
   }
   for(int i=0; i<M;i++){
    uthread_create(&(tConsumers[i]), Consumer, NULL);
   }
   for(int i=0; i<M;i++){
    uthread_join(tProducers[i]);
   }
   for(int i=0; i<M;i++){
    uthread_join(tConsumers[i]);
   }
   return 0;
}
