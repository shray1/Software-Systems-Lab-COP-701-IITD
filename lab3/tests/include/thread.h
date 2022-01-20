#ifndef THREAD_H
#define THREAD_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "signal.h"
#include "unistd.h"
#include <setjmp.h>
#include <time.h>
#include "queue.h"
#include <setjmp.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>

typedef void (*ThreadFunc)(void*);
struct Lock
{
    int isLocked;
    // At lock value is 1.
    // When free, value is 0.
};
struct Semaphore
{
    int iValue;        // value of the semaphore
    int iSemId;        // unique ID of semaphore
    Queue* qWaiters;  // blocked threads on the semaphore    
    int iMaxValuePossilbe;
};
struct TCB
{
    int iThreadId;
    jmp_buf tBuffer;   // saved context of this thread
    //signal_t *sig;     // signal that wakes up a waiting thread
    ThreadFunc pFuncToExecute;  //function pointer
    void* pThreadFuncArg;  // function argument
    int iWaitingForThread;
    int iStarted;

}; 
typedef struct Lock Lock;
typedef struct Semaphore Semaphore;
typedef struct TCB TCB;
typedef int uthread_t;

Lock* lInit();
void lAcquire(Lock* lock);
void lRelease(Lock* lock);

int uthread_create(uthread_t* t, ThreadFunc pFunc, void* pArg);
int uthread_yeild();
int uthread_join(uthread_t tid);
void t_exit(int val);
uthread_t uthread_getId();
int HasThreadFinished(uthread_t tid);

Semaphore* semInit();
void semV(Semaphore* sem);
void semP(Semaphore* sem);
void semErase(Semaphore* sem);

#endif





