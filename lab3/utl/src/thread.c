#include "thread.h"


#define MAX_THREADS 50 
TCB* gRunningThread=NULL;
TCB* gTcbMainThread=NULL;
jmp_buf gSchedulerContext;
jmp_buf gUserThreadStackSpaceContext;
Queue* gUserThreadQueue=NULL;
Queue* gFinishedThreads=NULL;
Queue* gBlockedThreads=NULL;
struct sigaction gSigAction;
struct itimerval gTimer;
sigset_t gBlockSigSet;
int gFlagBlockRunningThread;
int gSchedulerFirstRun;
int gUserThreadCount=0;
int gThreadStackCount=0;
ThreadFunc gThreadFunction;
int gSemaphoreIDCount=0;

TCB* InitaliaseTcbForNewThread(TCB* newThread,ThreadFunc pFunc,void*pArg);
int HasThreadFinished(uthread_t tid);
void InitialiseGlobalSettings();
void UnblockThreadsWithTid(uthread_t tid);
void UserThreadScheduler();
void AllocateUserThreadStackSpace();
void StartUserThread();
void DisableTimer();
void EnableTimer();
void AlarmHandler();


TCB* InitaliaseTcbForNewThread(TCB* newThread,ThreadFunc pFunc,void*pArg)
{
  //printf("DEBUG:: NewThread with id=%d\n", gUserThreadCount);
  newThread->iThreadId = gUserThreadCount++;
  newThread->iStarted = 0;
  newThread->pFuncToExecute = pFunc;
  newThread->pThreadFuncArg = pArg;
  newThread->iWaitingForThread = -1;
  return newThread;
}
//ignore attr for now. 
int uthread_create(uthread_t* t, ThreadFunc pFunc, void* pArg)
{
  if(gTcbMainThread == NULL){
    gUserThreadQueue = qInit();
  }
  DisableTimer();
  TCB* newThread = (struct TCB*)malloc(sizeof(struct TCB));
  newThread = InitaliaseTcbForNewThread(newThread,pFunc,pArg);
  qPush(gUserThreadQueue, newThread);
  *t = newThread->iThreadId;
  EnableTimer();
  if(gTcbMainThread == NULL){
    // One time effort. 
    //gUserThreadCount=0;
    gTcbMainThread = (struct TCB*)malloc(sizeof(struct TCB));
    gTcbMainThread = InitaliaseTcbForNewThread(gTcbMainThread,NULL,NULL);
    if(setjmp(gTcbMainThread->tBuffer)==0){
      qPush(gUserThreadQueue,gTcbMainThread);
      gTcbMainThread->iStarted = 1;
      InitialiseGlobalSettings();
      gRunningThread = gTcbMainThread;
      //Start Scheduler.
      DisableTimer();
      UserThreadScheduler();
    }else{
      //long Jump. do nothing. And get back to main.
    }
    
  }
  //enableInterrupts.
  return 0;
}
//1 : Failure,  0 : Success.
int uthread_yeild()
{
  //printf("DEBUG:: At thread_yeild()\n");
  DisableTimer();
  if(qIsEmpty(gUserThreadQueue)){
    return 1;     //Do Not Yeild. 
  }
    if (setjmp(gRunningThread->tBuffer)== 0) {
        qPush(gUserThreadQueue,(void*)gRunningThread);
        longjmp(gSchedulerContext, 1);
    } else { 
        DisableTimer();
        EnableTimer();
    }
    return 0;
}
//Later add return value.
// 0: represents success.
int uthread_join(uthread_t tid)
{
  //printf("DEBUG:: At thread_join()\n");
  DisableTimer();
  // Check if tid has already finished.
    if(HasThreadFinished(tid)){
      EnableTimer();
      return 0;
    }
    if (setjmp(gRunningThread->tBuffer)== 0) {
        //gRunningThread->iStarted=0;
        gRunningThread->iWaitingForThread=tid;
        //printf("DEBUG:: At thread_join() pushed into blocked thread. id=%d\n",gRunningThread->iThreadId);
        qPush(gBlockedThreads,gRunningThread);
        //Some way to tell the scheduler not to put 
        // current running thread in ready queue.
        gFlagBlockRunningThread = 1;
        longjmp(gSchedulerContext, 1);
    }
    EnableTimer();
    return 0;
}

void t_exit(int val) {
    //printf("DEBUG:: At t_exit() of thread id %d\n",gRunningThread->iThreadId);
    DisableTimer();
    qPush(gFinishedThreads,gRunningThread);
    UnblockThreadsWithTid(gRunningThread->iThreadId);
    //EnableTimer();
    longjmp(gSchedulerContext, 1);
}

uthread_t uthread_getId(){
  return gRunningThread->iThreadId;
}

int HasThreadFinished(uthread_t tid)
{
  // Check if tid is present in finishedThreadsQueue.
  //printf("DEBUG:: At HasThreadFinished()\n");
  int size = qSize(gFinishedThreads);
  int result = 0;
  for(int i=0; i<size; i++){
    if(result) break;
    TCB* temp = (TCB*)qPop(gFinishedThreads);
    if(temp->iThreadId == tid){
      result = 1;
    }
    qPush(gFinishedThreads,temp);
  }
  //printf("DEBUG:: At HasThreadFinished() Complete\n");
  return result;
}

void InitialiseGlobalSettings()
{
  //gUserThreadQueue = qInit();
  //printf("DEBUG:: At InitialiseGlobalSettings()\n");
  gBlockedThreads =  qInit();
  gFinishedThreads = qInit();
  memset(&gSigAction, 0, sizeof(gSigAction));
    gSigAction.sa_handler = &AlarmHandler;
    gSigAction.sa_flags = SA_NODEFER | SA_ONSTACK;
    sigaction(SIGVTALRM, &gSigAction, NULL);
    gTimer.it_value.tv_sec = 0;
    gTimer.it_value.tv_usec = 10000;
    gTimer.it_interval.tv_sec = 0;
    gTimer.it_interval.tv_usec = 10000;
    setitimer(ITIMER_VIRTUAL, & gTimer, NULL);
    if (sigaddset(&gBlockSigSet, SIGVTALRM) == -1) {
      //ERROR.
        return;
    }
}

void UnblockThreadsWithTid(uthread_t tid)
{
  
  //printf("DEBUG:: At UnblockThreadsWithTid()\n");
  //Remove threads from blocked state 
  // and bring them to ready state.
  int size = qSize(gBlockedThreads);
  for(int i=0; i<size; i++){
    TCB* temp = (TCB*)qPop(gBlockedThreads);
    if(temp->iWaitingForThread == tid){
      qPush(gUserThreadQueue,temp);
    }else{
      qPush(gBlockedThreads,temp);
    }
  }
}

void UserThreadScheduler()
{
    //printf("DEBUG:: At Scheduler()\n");
    if (gSchedulerFirstRun == 0)
    {
        gSchedulerFirstRun = 1; 
        if (setjmp(gSchedulerContext) == 0)
            AllocateUserThreadStackSpace();
        else{
            DisableTimer();
            //printf("DEBUG:: At SchedulerJump()\n");
        }
    }
    if(qIsEmpty(gUserThreadQueue)){
      /*No other thread in queue.
        Do nothing. End of scheduler.*/
    }else{
      gRunningThread = (TCB*)qPop(gUserThreadQueue);
      if (gRunningThread ->iStarted == 0) {
            gThreadStackCount++;
            gRunningThread->iStarted=1;
            StartUserThread();
        } else { 
          /*Thread has already been executing, jump to its location.*/
            longjmp(gRunningThread->tBuffer,1);
        }
    }
}

void AllocateUserThreadStackSpace(){
    //printf("DEBUG:: At AllocateUserThreadStackSpace()\n");
    int userSystemStackSpaceSize[5000 * MAX_THREADS]; 
    if (setjmp(gUserThreadStackSpaceContext) == 0)
        longjmp(gSchedulerContext, 1); 
    else {
        DisableTimer();
        longjmp(gSchedulerContext,1);
    }
}


void StartUserThread() {
    // Each thread occupies only (5000*(bytes of integer)) bytes.
    //printf("DEBUG:: At StartUserThread() with threadId =%d\n",gRunningThread->iThreadId);
    int singleThreadStack[5000 * gThreadStackCount];
    gThreadFunction = gRunningThread->pFuncToExecute; 
    EnableTimer();
    gThreadFunction(gRunningThread->pThreadFuncArg);  
    t_exit(0);
    //should not reach below too..
    longjmp(gSchedulerContext,1);
    // longjmp(gSchedulerContext, 1); 
    // never return back from this function as this will
    // corrupt the stack. so long jump to the scheduler 
    // and let it select some new thread to run.
}
void DisableTimer() {
    if (sigprocmask(SIG_BLOCK, &gBlockSigSet, NULL) == -1) {
        //ERROR.
    }
}
void EnableTimer() {
    if (sigprocmask(SIG_UNBLOCK, &gBlockSigSet, NULL) == -1) {
        //ERROR.
    }
}
void AlarmHandler() {
    //printf("DEBUG:: AlarmHandler() with threadId =%d\n",gRunningThread->iThreadId);
    DisableTimer();
    if (setjmp(gRunningThread->tBuffer) == 0) {
        qPush(gUserThreadQueue,gRunningThread);
        longjmp(gSchedulerContext, 1);
    } else { 
        DisableTimer();
        EnableTimer(); 
    }
}

////////////////////////////////////////////////////////////////////////
Lock* lInit(){
  Lock* lock;
  if((lock = (Lock*)malloc(sizeof(Lock))) == NULL)
  {
    //MallocError();
    return NULL;
  }
  lock->isLocked = 0;
  return lock;
}

void lAcquire(Lock* lock){
  int retVal = __sync_val_compare_and_swap(&(lock->isLocked)
                ,0,1); 
  while(retVal == 1){
    //Means the lock is already aquired by someone. 
    retVal = __sync_val_compare_and_swap(&(lock->isLocked)
                ,0,1);
  }
  // while(1){
  //   if(lock->isLocked == 0) break;
  // }
  // lock->isLocked = 1;
  DisableTimer();
}

void lRelease(Lock* lock){
    __sync_val_compare_and_swap(&(lock->isLocked),1,0);
    //lock->isLocked = 0;
    EnableTimer();
}

Semaphore* InitialiseValuesForSemaphore(Semaphore* sem,int count){
    sem->qWaiters=qInit();
    sem ->iValue = count;
    sem ->iSemId = gSemaphoreIDCount++;
    sem->iMaxValuePossilbe = count;
    return sem;
}

Semaphore* semInit(int count) {
    DisableTimer();
    Semaphore* semNew;
    if ((semNew = (Semaphore*) malloc(sizeof(Semaphore))) == NULL) {
        //ERROR.
        return NULL;
    }
    semNew = InitialiseValuesForSemaphore(semNew,count);
    EnableTimer();
    return semNew;
}

void semP(Semaphore* sem) {
    //printf("DEBUG:: semP() %d\n",sem->iSemId);
    DisableTimer();  // No Pre-emption.
    if(sem->iValue==0){
        if(setjmp(gRunningThread->tBuffer)==0){
            //Add in semaphore waiting list. 
            //printf("DEBUG:: semP() %d Waiting Queue\n",sem->iSemId);
            qPush(sem->qWaiters,gRunningThread);
            longjmp(gSchedulerContext,1);
            // If the user does not increase the value of this semphore
            // then the threads in the waiting queue will keep on waiting. 
            // Can this lead to Deadlock? 
            // Yes it can. Should be the users task to take care of deadlock 
            // which writing multi-threading code.
        }
        EnableTimer();
        //printf("DEBUG:: semP() %d GIVEN\n",sem->iSemId);
        return;      
    }
    sem->iValue--;
    //printf("DEBUG:: semP() %d GIVEN\n",sem->iSemId);
    EnableTimer();
}

void semV(Semaphore* sem) {
    //printf("DEBUG:: semV() %d\n",sem->iSemId);
    DisableTimer();
    if (qSize(sem->qWaiters) > 0) {
        //Push the waiting threads into ready queue. 
        // On first thoughts, this might seem as if this will cause a problem
        // ...if a thread is waiting on multiple semaphores then this will be an issue. 
        // BUT, note that since only one actual thread is running on the CPU
        // this will not be an issue.
        TCB* tempTcb = (TCB*)qPop(sem->qWaiters);
        qPush(gUserThreadQueue,tempTcb);
        //printf("DEBUG:: semV() %d Unblocked waiter.\n",sem->iSemId);
        EnableTimer();
        return;
    }
    //printf("DEBUG:: semV() %d Incremented Value.\n",sem->iSemId);
    sem->iValue++;
    if(sem->iValue > sem->iMaxValuePossilbe){
      sem->iValue = sem->iMaxValuePossilbe;
    }
    EnableTimer();
}

void semErase(Semaphore* sem) {
    DisableTimer();
    qErase(sem->qWaiters);
    free(sem);
    EnableTimer();
}

///////////////////////////////////////////////////////////////////////////////////
/* 


Important Learnings : 

1. Note that you must not use sigprocmask in multi-threaded processes,
   because each thread has its own signal mask and there is no single
   process signal mask. According to POSIX, the behavior of sigprocmask
   in a multi-threaded process is “unspecified”. Instead, use pthread_sigmask.




2. Two signals cannot be over-writter. SIGKILL and SIGSTOP.


*/


















