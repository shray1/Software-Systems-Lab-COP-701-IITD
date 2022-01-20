#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
int gBlockSize;
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
void blockMultiply(int**result, int**arr1, int**arr2, int size)
{
    int bsize = 10;
    //printf("Array size = %d\n",size);
    //printf("Block size = %d\n",bsize);
    for(int i=0; i<size; i+=bsize){
        for(int j=0; j<size; j+=bsize){
            for(int k=0; k<size; k+=bsize){
                //R[i][j] += A[i][k] * B[k][j]
                for(int a=i; a<i+bsize;a++){
                    for(int b=j;b<j+bsize;b++){
                        for(int c=0; c<bsize;c++){
                            if(a>=size || b>=size || c+k>=size){
                                continue;
                            }
                            result[a][b] += arr1[a][c+k]*arr2[c+k][b];
                        }
                    }
                }
            }
        }
    }

}
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
void createOutputDataForSimpleMultiplication()
{
    printf("Creating data for simple matrix multiplication"
        " for small arrays...\n");int n = 10;
    double dAvgTime = 0, dCpuTimeUsed;
    clock_t start,end;
    FILE *fp;
    fp = fopen("./data/simpleMultiplication.txt", "w");
    while(n < 400){
        int** res = allocateArray(n);
        int** arr1= allocateArray(n);
        int** arr2= allocateArray(n);
        fillArray(arr1,n);
        fillArray(arr2,n);
        for(int i=0; i<3; i++){
            resetArray(res,n);
            start = clock();
            multiply(res,arr1,arr2,n);
            end = clock();
            dCpuTimeUsed =  ((double) (end - start)) / CLOCKS_PER_SEC;
            dAvgTime += dCpuTimeUsed;
        }
        freeArray(res,n);
        freeArray(arr1,n);
        freeArray(arr2,n);
        dAvgTime /= 3;
        n+=10;
        fprintf(fp,"%d   %f\n",n, dAvgTime);
        printf("Matrix: %dx%d   SimpleMM=%fs\n",n,n,dAvgTime);
        dAvgTime = 0;
    }
    fclose(fp);
}
void createOutputDataForBlockMultiplication()
{
    printf("Creating data for block matrix multiplication"
        " for small arrays...\n");
    int n = 10;
    double dAvgTime = 0, dCpuTimeUsed;
    clock_t start,end;
    FILE *fp;
    fp = fopen("./data/blockMultiplication.txt", "w");
    while(n < 400){
        int** res = allocateArray(n);
        int** arr1= allocateArray(n);
        int** arr2= allocateArray(n);
        fillArray(arr1,n);
        fillArray(arr2,n);
        for(int i=0; i<3; i++){
            resetArray(res,n);
            start = clock();
            blockMultiply(res,arr1,arr2,n);
            end = clock();
            dCpuTimeUsed =  ((double) (end - start)) / CLOCKS_PER_SEC;
            dAvgTime += dCpuTimeUsed;
        }
        freeArray(res,n);
        freeArray(arr1,n);
        freeArray(arr2,n);
        dAvgTime /= 3;
        n+=10;
        fprintf(fp,"%d   %f\n",n, dAvgTime);
        printf("Matrix: %dx%d   BlockMM=%fs\n",n,n,dAvgTime);
        dAvgTime = 0;
    }
    fclose(fp);
}
void createOutputForLargeArrays()
{
    printf("Creating data for large arrays...\n");
    int n = 500;
    double dSimpleTime = 0, dBlockTime=0,dCpuTimeUsed;
    clock_t start,end;
    FILE *fp1,*fp2;
    fp1 = fopen("./data/largeArraySimpleMultiplication.txt", "w");
    fp2 = fopen("./data/largeArrayBlockMultiplication.txt", "w");
    while(n <= 2000){
        int** res = allocateArray(n);
        int** arr1= allocateArray(n);
        int** arr2= allocateArray(n);
        fillArray(arr1,n);
        fillArray(arr2,n);
        resetArray(res,n);
        start = clock();
        multiply(res,arr1,arr2,n);
        end = clock();
        dSimpleTime =  ((double) (end - start)) / CLOCKS_PER_SEC;
        resetArray(res,n);
        start = clock();
        blockMultiply(res,arr1,arr2,n);
        end = clock();
        dBlockTime =  ((double) (end - start)) / CLOCKS_PER_SEC;
        freeArray(res,n);
        freeArray(arr1,n);
        freeArray(arr2,n);
        fprintf(fp1,"%d   %f\n",n, dSimpleTime);
        fprintf(fp2,"%d   %f\n",n, dBlockTime);
        printf("Matrix: %dx%d, SimpleMM=%fs, BlockMM=%fs \n",
            n,n,dSimpleTime, dBlockTime);
        n+=100;
    }
    fclose(fp1);
    fclose(fp2);
}
void createOutputForBlockSizeVariation()
{
    int n = 1000;
    printf("Creating data for varying block size"
        " for a fixed matrix size (%dx%d).\n",n,n);
    double dBlockTime=0;
    clock_t start,end;
    FILE *fp;
    fp = fopen("./data/blockSizeVariation.txt", "w");
    int b=10;
    int** res = allocateArray(n);
    int** arr1= allocateArray(n);
    int** arr2= allocateArray(n);
    while(b <= 100){
        gBlockSize=b;
        fillArray(arr1,n);
        fillArray(arr2,n);
        resetArray(res,n);
        start = clock();
        blockMultiply(res,arr1,arr2,n);
        end = clock();
        dBlockTime =  ((double) (end - start)) / CLOCKS_PER_SEC;
        fprintf(fp,"%d   %f\n",b, dBlockTime);
        printf("Block size=%d  ,BlockMM time=%fs\n",b, dBlockTime);    
        b+=10;
    }
    freeArray(res,n);
    freeArray(arr1,n);
    freeArray(arr2,n);
    fclose(fp);
    gBlockSize = 10;
}
int verifyArraysAreEqual(int**arr1,int**arr2,int size){
    for(int i=0; i<size; i++){
        for(int j=0; j<size; j++){
            if(arr1[i][j]!=arr2[i][j]) return 0;
        }
    }
    return 1;
}
void singleIteration(int arrSize, int reps, int blocksize){
    printf("BlockMM : Block Matrix Multiplication\n"
                "SimpleMM : Simple Matrix Multiplication\n");
    int n = arrSize;
    double dAvgTime = 0, dCpuTimeUsed;
    clock_t start,end;
    int** resSM = allocateArray(n);
    int** resBM = allocateArray(n);
    int** arr1= allocateArray(n);
    int** arr2= allocateArray(n);
    fillArray(arr1,n);
    fillArray(arr2,n);
    for(int i=0; i<reps; i++){
        resetArray(resSM,n);
        start = clock();
        multiply(resSM,arr1,arr2,n);
        end = clock();
        dCpuTimeUsed =  ((double) (end - start)) / CLOCKS_PER_SEC;
        dAvgTime += dCpuTimeUsed;
    }
    dAvgTime /= reps;
    //printArray(resSM);
    printf("SimpleMM Time= %fs , for matrix: %dx%d.\n"
            ,dAvgTime, n,n);
    dAvgTime = 0;
    gBlockSize = blocksize;
    for(int i=0; i<reps; i++){
        resetArray(resBM,n);
        start = clock();
        blockMultiply(resBM,arr1,arr2,n);
        end = clock();
        dCpuTimeUsed =  ((double) (end - start)) / CLOCKS_PER_SEC;
        dAvgTime += dCpuTimeUsed;
    }
    dAvgTime /= reps;
    printf("BlockMM Time= %fs , for matrix: %dx%d.\n"
            ,dAvgTime, n,n);
    //printArray(resBM);
    if(verifyArraysAreEqual(resSM,resBM,n)==1){
        printf("SimpleMatrixMultiplication result array and "
            "BlockMatrixMultiplication result array are same.Verified OK.\n");
    }else{
        printf("SimpleMatrixMultiplication result array and "
            "BlockMatrixMultiplication result array are NOT same."
            "\n");
    }
    freeArray(resSM,n);
    freeArray(resBM,n);
    freeArray(arr1,n);
    freeArray(arr2,n);
    gBlockSize = 10;
}

void printUsage(){
    printf("USAGE :: This executable, expects 3 arguments.\n"
        "1- Matrix size.\n"
        "2- Number of repetitions.\n"
        "3- Block size.\n"
        "Example : ./mm 100 3 10\n\n");
}
int main(int argc, char**argv)
{
    // if(argc == 1){
    //     printUsage();
    //     return 0;
    // }
    if(argc == 2){
        if(strcmp(argv[1],"fromMakefile")==0){
            //Hidden from the user. Used by make.
            printf("BlockMM : Block Matrix Multiplication\n"
                "SimpleMM : Simple Matrix Multiplication\n");
            gBlockSize = 10;
            createOutputDataForSimpleMultiplication();
            createOutputDataForBlockMultiplication();
            createOutputForLargeArrays();
            createOutputForBlockSizeVariation();
            return 0;
        }
    }
    if(argc == 4){
        //User Input.
        int n,reps,blocksize; 
        n = (int)strtol(argv[1], NULL, 10);
        reps = (int)strtol(argv[2], NULL, 10);
        blocksize = (int)strtol(argv[3], NULL, 10);
        singleIteration(n,reps,blocksize);
        return 0;
    }
    printUsage();
    return 0;
}




