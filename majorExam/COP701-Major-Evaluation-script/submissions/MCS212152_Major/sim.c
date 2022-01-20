#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include<math.h>
#include<ctype.h>
#include<limits.h>

//For Inputs
char* gInputFileName=NULL;
char* gOutFileName=NULL;
char* gPolicy=NULL;
long** gCache=NULL;
int**  gDirtyBits=NULL;
int gMemoryBits = 32; // Given.
int gTagBits = -1;
int gSOBits = -1;  // Set-Offset Bits 
int gBlockSizeBits = -1;
int gBlockSize = -1;
int gNumOfBlocks = -1;
int gNumOfSets = -1;

//For Results.
int gNumOfStores = 0;
int gNumOfLoads = 0;
int gNumOfLoadHits = 0;
int gNumOfLoadMisses = 0;
int gNumOfStoreHits = 0;
int gNumOfStoreMisses = 0;
int gTotalCycles = 0;

void PrintUsage(char *argv[]);
void ReadArguments(char *argv[]);
void InitialiseGlobalVars(int iNumOfSets, int iNumOfBlocks, int iBlockSize);
void GetBinaryAddress(char* pMemoryAddress, char*pBinaryAddress);
int GetValueFromBinaryString(char* pBinStr, int iStart, int iEnd);
int IsTagValuePresentInCache(long lTagValue,int iSetIndex,int*pBlockLocation);
void  AddCycleCountsForDirtyBits();
void UpdateGlobalCount(int iIsStoreInst);
void UpdateGlobalMissesCount(int iIsStoreInst);
void UpdateGlobalHitsCount(int iIsStoreInst);
void PrintOutput();
void FreeGlobalVars();


// For FIFO 
int* gFIFOCounter = NULL;
void IterationFIFO(int iIsStore,long lTagValue,int iSetIndex);

// For LFU 
int** gFrequencyCounter=NULL;
int GetMinimumFrequencyIndex(int iSetIndex);
void IterationLFU(int iIsStore,long lTagValue,int iSetIndex);

// For LRU 
int gGlobalTime = 0;
int** gLastTimeUsed=NULL;
int GetLeastRecentlyUsedIndex(int iSetIndex);
void IterationLRU(int iIsStore,long lTagValue,int iSetIndex);



int main(int argc, char *argv[]){
    if(argc != 7){
        PrintUsage(argv);
        return 0;
    }
    ReadArguments(argv);
    FILE *inFile = fopen(gInputFileName, "r");
    if(inFile == NULL){
        printf("Could not open input file '%s'.\n", gInputFileName);
        return -1;
    }
    char* pLine = (char*)malloc(100*sizeof(char));
    while(1){
        if(fgets(pLine,100,inFile)){
            int iIsStoreInst = pLine[0]=='s'?1:0;
            char pMemoryAddress[9]={0};
            char pBinaryAddress[33] = {0};
            strncpy ( pMemoryAddress, pLine+4, 8);
            GetBinaryAddress(pMemoryAddress,pBinaryAddress);
            long lTagValue =  GetValueFromBinaryString(pBinaryAddress,0,gTagBits-1);
            //int iSetValue  =  GetValueFromBinaryString(pBinaryAddress,gTagBits,gTagBits+gSOBits-1);
            int iSetValue  =  GetValueFromBinaryString(pBinaryAddress,gTagBits,gTagBits+gSOBits-1);
            int iSetIndex  =  iSetValue;
            //printf("DEBUG :: %s , %s -> (0,%d)=%ld , (%d,%d)=%d \n", pMemoryAddress,pBinaryAddress,
            //              gTagBits, lTagValue, gTagBits+1, gSOBits, iSetValue);
            UpdateGlobalCount(iIsStoreInst);
            IterationFIFO(iIsStoreInst,lTagValue,iSetIndex);
            IterationLFU(iIsStoreInst,lTagValue,iSetIndex);
            IterationLRU(iIsStoreInst,lTagValue,iSetIndex);
        }else{
           break; 
           //EOF Reached.
        }
    }
    //AddCycleCountsForDirtyBits();
    /* Although the above call seems logical, but the testcases results 
        do not seem to be in tune with this call. So commenting it. */ 
    free(pLine);
    fclose(inFile);
    PrintOutput();
    FreeGlobalVars();
    return 0;
}

void PrintUsage(char *argv[]){
    fprintf(stderr, "Usage: %s <Number of Sets> <Number of Blocks> <Size of Blocks> <Eviction_Policy> <InputFile> <OutputFile> \n", argv[0]);
    fprintf(stderr, "   Number of Sets: Int\n"
                    "   Number of Blocks  : Int\n"
                    "   Size of Blocks  : Int\n"
                    "   Eviction Policy  : String\n"
                    "   Input File  : String\n"
                    "   Output File  : String\n");
}

void InitialiseGlobalVars(int iNumOfSets, int iNumOfBlocks, int iBlockSize){
    gMemoryBits = 32; //CONSTANT (GIVEN).
    gNumOfBlocks = iNumOfBlocks;
    gBlockSize  = iBlockSize;
    gNumOfSets = iNumOfSets;
    gBlockSizeBits = log(iBlockSize)/log(2);
    gSOBits = log(iNumOfSets)/log(2);
    gTagBits = gMemoryBits -  gBlockSizeBits - gSOBits;

    // printf("gNumOfBlocks = %d\n",iNumOfBlocks);
    // printf("gBlockSize = %d\n",iBlockSize);
    // printf("gNumOfSets = %d\n",iNumOfSets);
    // printf("gMemoryBits = %d\n",32);
    // printf("gBlockSizeBits = %d\n",gBlockSizeBits);
    // printf("gSOBits = %d\n",gSOBits);
    // printf("gTagBits = %d\n",gTagBits);

    gCache = (long**) calloc(iNumOfSets,sizeof(long*));
    for(int i=0; i<iNumOfSets; i++){
        gCache[i] = (long*) calloc(gNumOfBlocks,sizeof(long));
    }
    for(int i=0; i<iNumOfSets; i++){
        for(int j=0; j<gNumOfBlocks; j++){
            gCache[i][j] = -1;  //Denotes it is free initially.
        }
    }
    gDirtyBits = (int**) calloc(iNumOfSets,sizeof(int*));
    for(int i=0; i<iNumOfSets; i++){
        gDirtyBits[i] = (int*) calloc(gNumOfBlocks,sizeof(int));
    }
    for(int i=0; i<iNumOfSets; i++){
        for(int j=0; j<gNumOfBlocks; j++){
            gDirtyBits[i][j] = 0;  //Denotes it is clean initially.
        }
    }

    gFIFOCounter = (int*) calloc(iNumOfSets,sizeof(int));
    for(int i=0; i<iNumOfSets; i++){
        gFIFOCounter[i] = 0;
    }

    gFrequencyCounter = (int**) calloc(iNumOfSets,sizeof(int*));
    for(int i=0; i<iNumOfSets; i++){
        gFrequencyCounter[i] = (int*) calloc(gNumOfBlocks,sizeof(int));
    }
    for(int i=0; i<iNumOfSets; i++){
        for(int j=0; j<gNumOfBlocks; j++){
            gFrequencyCounter[i][j] = 0; // Initially 0 is the frequency.
        }
    }

    gLastTimeUsed = (int**) calloc(iNumOfSets,sizeof(int*));
    for(int i=0; i<iNumOfSets; i++){
        gLastTimeUsed[i] = (int*) calloc(gNumOfBlocks,sizeof(int));
    }
    for(int i=0; i<iNumOfSets; i++){
        for(int j=0; j<gNumOfBlocks; j++){
            gLastTimeUsed[i][j] = -1; 
            // Start time is 0. Everyone is initialised to time before 0. 
        }
    }
}

void Free2DArray(int** pArr){
    for(int i=0; i<gNumOfSets; i++){
        free(pArr[i]);
    }
    free(pArr);
}

void FreeGlobalVars(){
    Free2DArray(gLastTimeUsed);
    Free2DArray(gFrequencyCounter);
    Free2DArray(gDirtyBits);
    for(int i=0; i<gNumOfSets; i++){
        free(gCache[i]);
    }
    free(gCache);
    free(gFIFOCounter);
}



int IsTagValuePresentInCache(long lTagValue,int iSetIndex,int*pBlockLocation){
    for(int i=0; i<gNumOfBlocks; i++){
        if(gCache[iSetIndex][i] == lTagValue){
            *pBlockLocation=i;
            return 1;
        }
    }
    return 0;
}

int GetMinimumFrequencyIndex(int iSetIndex){
    int iValue = INT_MAX;
    int iResult = -1;
    for(int i=0; i<gNumOfBlocks; i++){
        if(gFrequencyCounter[iSetIndex][i] < iValue){
            iValue=gFrequencyCounter[iSetIndex][i];
            iResult=i;
        }
    }
    return iResult;
}

int GetLeastRecentlyUsedIndex(int iSetIndex){
    int iValue = INT_MAX;
    int iResult = -1;
    for(int i=0; i<gNumOfBlocks; i++){
        if(gLastTimeUsed[iSetIndex][i] < iValue){
            iValue=gLastTimeUsed[iSetIndex][i];
            iResult=i;
        }
    }
    return iResult;
}
int GetValueFromBinaryString(char* pBinStr, int iStart, int iEnd){
    int iValue = pBinStr[iEnd] - '0';
    iEnd--;
    int temp = 2;
    while(iEnd>=iStart){
        if(pBinStr[iEnd]=='1'){
            iValue = iValue + temp;
        }
        temp *= 2;
        iEnd--;
    }
    return iValue;
}

void  AddCycleCountsForDirtyBits(){
    for(int i=0; i<gNumOfSets; i++){
        for(int j=0; j<gNumOfBlocks; j++){
            if(gDirtyBits[i][j] ==1){
                gTotalCycles +=  (gBlockSize/4)*100 ;
            }
        }
    }
}

void GetBinaryAddress(char* pMemoryAddress, char*pBinaryAddress){
    for(int i=0; i<8; i++){
        switch(pMemoryAddress[i]){
            case '0' : strcat(pBinaryAddress,"0000");
                break;
            case '1' : strcat(pBinaryAddress,"0001");
                break;  
            case '2' : strcat(pBinaryAddress,"0010");
                break;
            case '3' : strcat(pBinaryAddress,"0011");
                break;
            case '4' : strcat(pBinaryAddress,"0100");
                break;
            case '5' : strcat(pBinaryAddress,"0101");
                break;
            case '6' : strcat(pBinaryAddress,"0110");
                break;
            case '7' : strcat(pBinaryAddress,"0111");
                break;
            case '8' : strcat(pBinaryAddress,"1000");
                break;
            case '9' : strcat(pBinaryAddress,"1001");
                break;
            case 'A' : strcat(pBinaryAddress,"1010");
                break;
            case 'B' : strcat(pBinaryAddress,"1011");
                break;
            case 'C' : strcat(pBinaryAddress,"1100");
                break;
            case 'D' : strcat(pBinaryAddress,"1101");
                break;
            case 'E' : strcat(pBinaryAddress,"1110");
                break;
            case 'F' : strcat(pBinaryAddress,"1111");
                break;
        }
    }  
}

void IterationFIFO(int iIsStore,long lTagValue,int iSetIndex){
    if(strcmp(gPolicy , "FIFO") == 0){
        int iBlockLocation = -1;
        if(IsTagValuePresentInCache(lTagValue,iSetIndex,&iBlockLocation)){
            // Hit.
            if(iIsStore){
                // Set Dirty Bit.
                // Because its already present in cache,
                // and we are updating in the cache.
                gDirtyBits[iSetIndex][iBlockLocation]=1; //Set Dirty Bit.
            }
            UpdateGlobalHitsCount(iIsStore);
        }else{
            //Miss
            gCache[iSetIndex][gFIFOCounter[iSetIndex]] = lTagValue;
            if(gCache[iSetIndex][gFIFOCounter[iSetIndex]] != -1){
                // Was Occupied, Add Cycles if it's dirty.
                if(gDirtyBits[iSetIndex][gFIFOCounter[iSetIndex]]==1){
                    gTotalCycles += (gBlockSize/4)*100;
                    gDirtyBits[iSetIndex][gFIFOCounter[iSetIndex]] = 0;
                }
            }
            gTotalCycles += (gBlockSize/4)*100;
            UpdateGlobalMissesCount(iIsStore);
            gFIFOCounter[iSetIndex] = (gFIFOCounter[iSetIndex]+1) % gNumOfBlocks;
        }
        gTotalCycles++;
    }
}

void IterationLFU(int iIsStore,long lTagValue,int iSetIndex){
    if(strcmp(gPolicy , "LFU") == 0){
        int iBlockLocation = -1;
        if(IsTagValuePresentInCache(lTagValue,iSetIndex,&iBlockLocation)){
            // Hit.
            gFrequencyCounter[iSetIndex][iBlockLocation]++;
            UpdateGlobalHitsCount(iIsStore);
            if(iIsStore){
                // Set Dirty Bit.
                // Because its already present in cache,
                // and we are updating in the cache.
                gDirtyBits[iSetIndex][iBlockLocation]=1; 
            }
        }else{
            //Miss
            int iMinFreqIndex = GetMinimumFrequencyIndex(iSetIndex);
            if(gFrequencyCounter[iSetIndex][iMinFreqIndex] > 0){
                //It was already occupied. Add Cycles if it's dirty.
                if(gDirtyBits[iSetIndex][iMinFreqIndex]==1){
                    gTotalCycles += (gBlockSize/4)*100;
                    gDirtyBits[iSetIndex][iMinFreqIndex] = 0;
                }
            }
            gTotalCycles += (gBlockSize/4)*100; //Due to a miss.
            gFrequencyCounter[iSetIndex][iMinFreqIndex]=1;
            gCache[iSetIndex][iMinFreqIndex]=lTagValue;
            UpdateGlobalMissesCount(iIsStore);
        }
        gTotalCycles++;
    }
}

void IterationLRU(int iIsStore,long lTagValue,int iSetIndex){
    if(strcmp(gPolicy , "LRU") == 0){
        int iBlockLocation = -1;
        if(IsTagValuePresentInCache(lTagValue,iSetIndex,&iBlockLocation)){
            // Hit.
            gLastTimeUsed[iSetIndex][iBlockLocation]=gGlobalTime;
            UpdateGlobalHitsCount(iIsStore);
            if(iIsStore){
                gDirtyBits[iSetIndex][iBlockLocation]=1; 
                // Set Dirty Bit.
                // Because its already present in cache,
                // and we are updating in the cache.
            }
        }else{
            // Miss
            int iLRUIndex = GetLeastRecentlyUsedIndex(iSetIndex);
            if(gLastTimeUsed[iSetIndex][iLRUIndex] >= 0){
                //It was already occupied and is dirty.
                if(gDirtyBits[iSetIndex][iLRUIndex]==1){
                    gTotalCycles += (gBlockSize/4)*100;  // To make up for dirty bit.
                    gDirtyBits[iSetIndex][iLRUIndex] = 0;
                }
            }
            gTotalCycles += (gBlockSize/4)*100; //Due to a miss.
            gLastTimeUsed[iSetIndex][iLRUIndex]=gGlobalTime;
            gCache[iSetIndex][iLRUIndex]=lTagValue;
            UpdateGlobalMissesCount(iIsStore);
        }
        gTotalCycles++;
        gGlobalTime++; 
    }
}

void PrintOutput(){
    FILE *pFile = fopen(gOutFileName, "w");
    if(pFile == NULL){
        printf("Could not open output file '%s'.\n", gOutFileName);
        return;
    }
    fprintf(pFile,"Total loads: %d\n", gNumOfLoads);
    fprintf(pFile,"Total stores: %d\n", gNumOfStores);
    fprintf(pFile,"Load hits: %d\n", gNumOfLoadHits);
    fprintf(pFile,"Load misses: %d\n", gNumOfLoadMisses);
    fprintf(pFile,"Store hits: %d\n", gNumOfStoreHits);
    fprintf(pFile,"Store misses: %d\n", gNumOfStoreMisses);
    fprintf(pFile,"Total cycles: %d\n", gTotalCycles);
    fclose(pFile);
}

void UpdateGlobalCount(int iIsStoreInst){
    if(iIsStoreInst){
        gNumOfStores++;
    }else{
        gNumOfLoads++;
    }
}

void UpdateGlobalHitsCount(int iIsStoreInst){
    if(iIsStoreInst){
        gNumOfStoreHits++;
    }else{
        gNumOfLoadHits++;
    }
}
void UpdateGlobalMissesCount(int iIsStoreInst){
    if(iIsStoreInst){
        gNumOfStoreMisses++;
    }else{
        gNumOfLoadMisses++;
    }
}

void ReadArguments(char *argv[]){
    int iNumOfSets = atoi(argv[1]);
    int iNumOfBlocks = atoi(argv[2]);
    int iBlockSize = atoi(argv[3]);
    gPolicy = argv[4];
    gInputFileName = argv[5];
    gOutFileName = argv[6];
    InitialiseGlobalVars(iNumOfSets,iNumOfBlocks,iBlockSize);
}


//Unused make file.
/*
OBJDIR :=./obj
BINDIR :=./bin

all : ./bin/sim
sim : ./bin/sim 
./bin/sim : ./obj/sim.o
    @[ -d $(BINDIR) ] || mkdir $(BINDIR)
    gcc -o ./bin/sim ./obj/sim.o 

./obj/sim.o : ./src/sim.c
    @[ -d $(OBJDIR) ] || mkdir $(OBJDIR)
    gcc -c ./src/sim.c  -o ./obj/mlp.o -Wall -Wextra -pedantic -g  -lm 

run : ./bin/sim
    ./sim $(num_sets) $(num_blocks) $(size_block) $(eviction_policy) $(trace_file) $(output_file)

clean : 
    @echo "Cleaning files."
    @rm -rf $(BINDIR)
    @rm -rf $(OBJDIR)
    @echo "Files cleaned."
*/



