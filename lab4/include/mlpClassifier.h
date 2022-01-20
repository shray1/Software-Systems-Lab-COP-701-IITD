#ifndef MLP_CLASSIFIER_H
#define MLP_CLASSIFIER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

struct MlpClassifier{
    int iHiddenLayers_;
    int iHLSize_;  //EachHiddenLayerSize
    int* pLayersSize_;
    //int* hidden_activation_functions;
    int iActivationFunction_; 
    // 0: Sigmoid
    // 1: Tanh
    // 2: Relu
    int iCostFunction_; 
    // 0: MSE 
    // 1: CE
    int iBackpropAlgo_;
    // 0: S-GD 
    // 1: FullGD
    // 2: MB-GD
    double dLearningRate_;
    int iMaxIterations_;
    //int momentum;
    int iOLSize_;  //OutputLayerSize
    //int output_activation_function;
    double** pData_;

    int iNumberOfRows_;
    int iNumberOfFeatures_;

    int iMinTrainIndex_;
    int iMaxTrainIndex_;
    int iMinTestIndex_;
    int iMaxTestIndex_;
    double*** pWeights_;
    int iIsRegression_;
};
typedef struct MlpClassifier MlpClassifier;
MlpClassifier* GetMLPClassifier(int iHiddenLayers, int iHLSize, int iActivationFunction,
                    int iCostFunction, int iBackpropAlgo, double dLearningRate,
                    int iMaxIterations, int iOLSize, double** pData, int iNumberOfRows,
                    int iNumberOfFeatures, double dPercentTraining, int iIsRegression );
void TrainModel(MlpClassifier* pModel);
void ClassifyData(MlpClassifier* pModel);
void PrintModelWeights(MlpClassifier* pModel);
int ReadInputData(char* pFileName, int iNumOfRows, int iNumOfFeatures, double** pData);
#endif





