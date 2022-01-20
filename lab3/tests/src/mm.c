#include<math.h>
#include <pthread.h>
#include <semaphore.h>
#include<stdio.h>
#include<stdlib.h>
#include "thread.h"

#define n 180
//int n = 16;
// MT = MyThread
// PT = PThread
int** gResultMT;
int** gResultPT;
int** gArr1;
int** gArr2;
sem_t gPTSem;
Semaphore*  gMTSem;
int gMTCount;
int gPTCount;
int gMTWorkDivision;
int gPTWorkDivision;
void InitialiseGlobalVars();

void printArray(int** arr, int size)
{
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            if(j>0)printf(",%d",arr[i][j]);
            else printf("%d",arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
void fillArray(int** arr, int size){
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            arr[i][j]=rand()%100;
        }
    }
}
void resetArray(int** arr, int size){
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            arr[i][j]=0;
        }
    }
}
void freeArray(int**arr, int size){
    for(int i=0; i<size; i++){
        free(arr[i]);
    }
    free(arr);
}
int** allocateArray(int size){
    int** arr = (int**) calloc(size,sizeof(int*));
    for(int i=0; i<size; i++){
        arr[i] = (int*) calloc(size,sizeof(int));
    }
    return arr;
}
int verifyArraysAreEqual(int**arr1,int**arr2,int size){
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            if(arr1[i][j]!=arr2[i][j]) return 0;
        }
    }
    return 1;
}


void* PTJob(void* dummy)
{
    sem_wait(&gPTSem);
    int start = n/gPTCount * gPTWorkDivision;
    int end   = n/gPTCount + start;
    gPTWorkDivision++;
    sem_post(&gPTSem);
    for(int row=start; row<end; row++){
        for(int i=0; i<n; i++){
            for(int j=0; j<n;j++){
                gResultPT[row][i] += gArr1[row][j]*gArr2[j][i];
            }
        }
    }
    return NULL;
}
void MMUsingPthreads()
{
    sem_init(&gPTSem, 0, 1);
    pthread_t* threads = (pthread_t*) malloc (gPTCount*sizeof(pthread_t));
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    for(long lThread=0; lThread<gPTCount;lThread++){
        pthread_create(&threads[lThread],&attr,PTJob,NULL);
    }
    for(long i=0 ;i<gPTCount;i++){
        pthread_join(threads[i],NULL);
    }
}


void MTJob(void* dummy)
{
    sem_wait(&gPTSem);
    int start = n/gMTCount * gMTWorkDivision;
    int end   = n/gMTCount + start;
    gMTWorkDivision++;
    sem_post(&gPTSem);
    for(int row=start; row<end; row++){
        for(int i=0; i<n; i++){
            for(int j=0; j<n;j++){
                gResultMT[row][i] += gArr1[row][j]*gArr2[j][i];
            }
        }
    }
}

void MMUsingMyThreads()
{
    gMTSem = semInit(1);
    uthread_t tWorkers[gMTCount];
    for(int i=0; i<gMTCount;i++){
        uthread_create(&(tWorkers[i]), MTJob, NULL);
    }
    for(int i=0; i<gMTCount;i++){
        uthread_join(tWorkers[i]);
    }
}


void multiply(int** result, int **arr1, int**arr2, int size)
{
    for(int i=0; i< size; i++)
    {
        for(int j=0; j< size; j++)
        {
            for(int k=0; k<size; k++){
                result[i][j] += arr1[i][k] * arr2[k][j];
            }
        }
    }
}

int main(int argc, char**argv)
{
    //PlotThreadVsTimeData();
    InitialiseGlobalVars();
    if(n%gMTCount != 0){
        printf("ERROR. Size of array should be divisible by"
            " the number of threads.\n");
        return 1;
    }
    double dCpuTimeUsed;
    clock_t start,end;

    start = clock();
    MMUsingPthreads();
    end = clock();
    dCpuTimeUsed =  ((double) (end - start)) / CLOCKS_PER_SEC;
    double dCpuTimeUsedPT = dCpuTimeUsed;

    start = clock();
    MMUsingMyThreads();
    end = clock();
    dCpuTimeUsed =  ((double) (end - start)) / CLOCKS_PER_SEC;
    double dCpuTimeUsedMT = dCpuTimeUsed;

    if(verifyArraysAreEqual(gResultMT,gResultPT,n)==0){
        printf("Arrays Not Equal!!! \n\n");
    }
    printf("Time taken to compute matrix multiplication using"
        " %d thread for %dx%d matrix -> \n",gMTCount,n,n);
    printf("Using Pthreads : %f\n", dCpuTimeUsedPT);
    printf("Using Mythreads : %f\n", dCpuTimeUsedMT);
    //multiply(gResultMT, gArr1, gArr2,n);
    //printf("Arr1 = \n");
    // printArray(gArr1,n);
    // printf("Arr2 = \n");
    // printArray(gArr2,n);
    // printf("Result = \n");
    //printArray(gResultPT,n);
    //printArray(gResultMT,n);
    return 0;
}

void InitialiseGlobalVars()
{
    gArr1= allocateArray(n);
    gArr2= allocateArray(n);
    fillArray(gArr1,n);
    fillArray(gArr2,n);
    gResultMT = allocateArray(n);
    resetArray(gResultMT,n);
    gResultPT = allocateArray(n);
    resetArray(gResultPT,n);
    gMTCount =5;
    gPTCount =5;
}

