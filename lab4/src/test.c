#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "mlpClassifier.h"


double ** FetchInputData(char* pFileName, int iNoOfFeatures, int iNoOfRows);
void PrintUsage();

int main(int argc, char** argv) {
    /*
        argv[0]: Executable file name Ex: a.out
        argv[1]: Backpropogation Algorithm 
                 O for Schtocastic Gradient Descent 
                 1 for Mini Batch Gradient Descent 
                 2 for Batch Gradient Descent 
        argv[2]: Activation Function 
                 0 for Sigmoid Function
                 1 for TanH function 
                 2 for ReLu function 
        argv[3]: Cost Function 
                 0 for Mean Squared Error 
                 1 for Cross Entropy Error 
        argv[4]: Number of Hidden Layers 
        argv[5]: Number of Nodes in each hidden layer     
        argv[6]: Learning Rate 
        argv[7]: Number of Iterations 
        argv[8]: Number of Units in Output Layer 
        argv[9]: Input Data File Name. 
                -> Data file should only consists of numbers. 
                -> First column of data should represent the output values. 
                -> Rest of the columns should be the data for input layers.
        argv[10]: Number of Total Features in the data file. (Including the output feature.)
        argv[11]: Number of Total Rows in InputFile
        argv[12]: Fraction of total rows, which are to be kept for testing purpose.
        argv[13]: Classification Or Regression
                   0 for Classification
                   1 for Regression
    */  
    if(argc != 14){
        PrintUsage();
        return 0;
    }
    int iBackAlgo = atoi(argv[1]);
    int iActFunc = atoi(argv[2]);
    int iCostFunc = atoi(argv[3]);
    int iNoOfHiddenLayers = atoi(argv[4]);
    int iNoOfNodesHL = atoi(argv[5]);
    double dLearningRate = atof(argv[6]);
    int iMaxIteration = atoi(argv[7]);
    int iOutputLayerNodes = atoi(argv[8]);
    char *pInputDataFileName = argv[9];
    int iNumOfFeatures = atoi(argv[10]);
    int iNumOfRows = atoi(argv[11]);
    double dFractionTestingData = atof(argv[12]);
    double** pData = FetchInputData(pInputDataFileName,iNumOfFeatures,iNumOfRows);
    int iClassOrRegr = atoi(argv[13]);
    if(pData==NULL){
        printf("Could not fetch input data. Aborting . . .\n");
        return 0;
    }
    
    // MlpClassifier* GetMLPClassifier(iHiddenLayers,iHLSize,iActivationFunction,
    //                 iCostFunction,iBackpropAlgo,dLearningRate,
    //                 iMaxIterations, iOLSize,pData, iNumberOfRows,
    //                 iNumberOfFeatures, dPercentTraining )
    MlpClassifier* pModel = GetMLPClassifier(iNoOfHiddenLayers,iNoOfNodesHL,
                    iActFunc,iCostFunc,iBackAlgo,dLearningRate,
                    iMaxIteration,iOutputLayerNodes, 
                    pData,iNumOfRows,iNumOfFeatures, dFractionTestingData,iClassOrRegr);
    TrainModel(pModel);
    ClassifyData(pModel);
    return 0;
}

void PrintUsage(){
    printf("Usage : \n");
    printf("./mlp <1-int> <2-int> <3-int> <4-int> <5-int> <6-float>"
        "     <7-int> <8-int> <9-string> <10-int> <11-int> <12-int>\n");
    printf("Argument 1 : Backpropogation Algorithm\n");
    printf("              O for Schtocastic Gradient Descent\n");
    printf("              1 for Mini Batch Gradient Descent\n");
    printf("              2 for Batch Gradient Descent\n");
    printf("Argument 2 : Activation Function \n");
    printf("              0 for Sigmoid Function\n");
    printf("              1 for TanH function\n");
    printf("              2 for ReLu function\n");
    printf("Argument 3 : Cost Function \n");
    printf("              0 for Mean Squared Error\n");
    printf("              1 for Cross Entropy Error\n");
    printf("Argument 4 : Number of Hidden Layers \n");
    printf("Argument 5 : Number of Nodes in each hidden layer \n");
    printf("Argument 6 : Learning Rate \n");
    printf("Argument 7 : Number of Iterations \n");
    printf("Argument 8 : Number of Units in Output Layer\n");
    printf("Argument 9 : Input Data File Name \n");
    printf("Argument 10 : Number of Total Features in the data file. (Including the output feature.) \n");
    printf("Argument 11 : Number of Total Rows in InputFile \n");
    printf("Argument 12 : Fraction of total rows, which are to be kept for testing purpose. \n");
    printf("               It's value should be between 0 and 1. \n");
    printf("Argument 13 : Classification or Regression\n");
    printf("               0 for Classification\n");
    printf("               1 for Regression\n");
}
double ** FetchInputData(char* pFileName, int iNoOfFeatures, int iNoOfRows){
    char* pInputDataFileName = pFileName;
    int iNumOfFeatures = iNoOfFeatures;
    int iNumOfRows = iNoOfRows;

    double** pInputData = (double**)malloc(iNumOfRows * sizeof(double*));
    for (int i = 0; i < iNumOfRows; i++){
        pInputData[i] = (double*)malloc(iNumOfFeatures * sizeof(double));
    }

    if(ReadInputData(pInputDataFileName,iNumOfRows,iNumOfFeatures,pInputData)==-1){
        //ERROR.
        return NULL;
    }

    return pInputData;
}