#include "mlpClassifier.h"


//Forward_Propogate Helper Functions
void Forward_Propogate(MlpClassifier* pModel, int iTrainingIndex, 
    double** pLayersInputCache, double** pLayersOutputCache);
double** AllocateLayersOutputCache(MlpClassifier* pModel);
double** AllocateLayersInputCache(MlpClassifier* pModel);
void MatrixMultiply(double* pInputArr, double** pWeightsArr, double* pOuputArr, int iInputSize, int iOutputSize);
void SigmoidFunc(int iSize, double* pInputArr, double* pOuputArr);
void TanhFunc(int iSize, double* pInputArr, double* pOuputArr);
void ReLuFunc(int iSize, double* pInputArr, double* pOuputArr);
void SoftmaxFunc(int iSize, double* pInputArr, double* pOuputArr);

//Backward Propogate Helper Functions
void Backward_Propogate(MlpClassifier* pModel, int iTrainingDataIndex, 
    double** pLayersInputCache, double** pLayersOutputCache);
void Backward_Propogate_Batch(MlpClassifier* pModel, int iTrainingDataIndex, 
    double** pLayersInputCache, double** pLayersOutputCache, double*** pWeightCorrectionArr);
void DerivativeSigmoid(int iLayerSize, double* layer_output, double* pLayerDerivativeArr);
void DerivativeTanh(int iLayerSize, double* pLayerOutput, double* pLayerDerivativeArr);
void DerivativeReLu(int iLayerSize, double* pLayerInput, double* pLayerDerivativeArr);
void DerivativeSoftmax(int iLayerSize, double* pLayerOutput, double* pLayerDerivativeArr);
double*** AllocateWeightsCorrectionMatrix(int iTotalLayers, int* pLayerSizes);
double* AllocateExpectedOutputArray(MlpClassifier* pModel, int iDataIndex);
double** AllocateGradientMatrix(MlpClassifier* pModel);
void ComputeGradientsForLastLayer(MlpClassifier* pModel,double** pNodesGradientArr,double* pExpectOutputArr, double** pLayersInputCache,
                double** pLayersOutputCache);
void ComputeWeightCorrectionForLastLayer(MlpClassifier* pModel,double*** pWeightCorrectionArr,double** pNodesGradientArr,
                    double** pLayersOutputCache);
void AddWeightCorrectionForLastLayer(MlpClassifier* pModel,double*** pWeightCorrectionArr,double** pNodesGradientArr,
                    double** pLayersOutputCache);
void ComputeGradientsForHiddenLayers(MlpClassifier* pModel,double** pNodesGradientArr,double* pExpectOutputArr, double** pLayersInputCache,
                double** pLayersOutputCache);
void ComputeWeightCorrectionForHiddenLayers(MlpClassifier* pModel,double*** pWeightCorrectionArr,double** pNodesGradientArr,
                    double** pLayersOutputCache);
void AddWeightCorrectionForHiddenLayers(MlpClassifier* pModel,double*** pWeightCorrectionArr,double** pNodesGradientArr,
                    double** pLayersOutputCache);
void AverageWeightCorrections(MlpClassifier* pModel,double*** pWeightCorrectionArr,int iBatchSize);
void ApplyWeightCorrections(MlpClassifier* pModel, double*** pWeightCorrectionArr);
void FreeGradientMatrix(MlpClassifier* pModel, double** pNodesGradientArr);
void FreeWeightCorrectionMatrix(MlpClassifier* pModel,double*** pWeightCorrectionArr);
 

// --Debug functions.
void PrintInputOutputCache(MlpClassifier* pModel,double** pArrOut, double** pArrIn);


// Other Functions.
void SGDTrainModel(MlpClassifier* pModel);
void BatchTrainModel(MlpClassifier* pModel, int iBatchSize);
void ClassifyBinaryTestData(MlpClassifier* pModel);
void ClassifyRegressionTestData(MlpClassifier* pModel);


// Print Loss Data Functions.
void WriteLossData(MlpClassifier* pModel, double*pData);
void ComputeLossMSE(MlpClassifier* pModel,double dOutputVal,
        double* pLossData, int iTrainingIndex ,int iCount);
void AddComputeLossMSE(MlpClassifier* pModel,double dOutputVal,
        double* pLossData, int iTrainingIndex ,int iCount);
void MeanComputeLossMSE(MlpClassifier* pModel,double* pLossData, int n);



void WriteLossData(MlpClassifier* pModel, double*pData)
{
    char* pAlgo = "sdg";
    if(pModel->iBackpropAlgo_ == 1 ){
        pAlgo = "batch";
    }
    if(pModel->iBackpropAlgo_ == 2 ){
        pAlgo = "mini_batch";
    }

    char* pActFun = "sig";
    if(pModel->iActivationFunction_ == 1){
        pActFun = "tanh";
    }
    if(pModel->iActivationFunction_ == 2){
        pActFun = "relu";
    }

    char* pCost = "mse";
    if(pModel->iCostFunction_==1){
        pCost = "ce";
    }

    char* pRC = "cls";
    if(pModel->iIsRegression_==1){
        pRC = "reg";
    }
    char fileName[256];
    sprintf(fileName,"%s_%s_%s_%s.txt",pAlgo,pActFun,pCost,pRC);

    int n = pModel->iMaxIterations_;
    FILE* pFile = fopen(fileName, "w");
    for(int i=0; i<n; i++)
    {
        fprintf(pFile, "%d    %f\n", i , pData[i]);
    }
    fclose(pFile);
}

void ComputeLossMSE(MlpClassifier* pModel,double dOutputVal,
        double* pLossData, int iTrainingIndex ,int iCount)
{
    double dTemp = pModel->pData_[iTrainingIndex][0] - dOutputVal;
    pLossData[iCount] = (dTemp*dTemp)*(1.0/2.0);
}

void AddComputeLossMSE(MlpClassifier* pModel,double dOutputVal,
        double* pLossData, int iTrainingIndex ,int iCount)
{
    double dTemp = pModel->pData_[iTrainingIndex][0] - dOutputVal;
    pLossData[iCount] += (dTemp*dTemp)*(1.0/2.0);
}
void MeanComputeLossMSE(MlpClassifier* pModel,double* pLossData, int n)
{
    int X = pModel->iMaxIterations_;
    for(int x=0; x<X; x++)
    {
        pLossData[x] = pLossData[x]/n;
    }
}



MlpClassifier* GetMLPClassifier(int iHiddenLayers, int iHLSize, int iActivationFunction,
                    int iCostFunction, int iBackpropAlgo, double dLearningRate,
                    int iMaxIterations, int iOLSize, double** pData, int iNumberOfRows,
                    int iNumberOfFeatures, double dPercentTraining, int iIsRegression )
{
    MlpClassifier* pModel = (MlpClassifier*)malloc(sizeof(MlpClassifier));
    pModel->iHiddenLayers_ = iHiddenLayers;
    pModel->iHLSize_ = iHLSize;
    int* pTempSizeLayers = (int*)malloc((iHiddenLayers+2)*sizeof(int));
    pTempSizeLayers[0] = iNumberOfFeatures -1;
    for(int i=1; i<iHiddenLayers+1; i++){
        pTempSizeLayers[i] = iHLSize;
    }
    pTempSizeLayers[iHiddenLayers+1] = 1; //Always Binary Classification?
    pModel->pLayersSize_ = pTempSizeLayers;
    pModel->iActivationFunction_ = iActivationFunction;
    pModel->iCostFunction_ = iCostFunction;
    pModel->iBackpropAlgo_ = iBackpropAlgo;
    pModel->dLearningRate_ = dLearningRate;
    pModel->iMaxIterations_= iMaxIterations;
    pModel->iOLSize_ = iOLSize;
    pModel->pData_ = pData;
    pModel->iNumberOfRows_ = iNumberOfRows;
    pModel->iNumberOfFeatures_ = iNumberOfFeatures;
    int temp = (dPercentTraining * iNumberOfRows);
    pModel->iMinTrainIndex_ = 0;
    pModel->iMaxTrainIndex_ = temp;
    pModel->iMinTestIndex_ = temp+1;
    pModel->iMaxTestIndex_ = iNumberOfRows -1;
    // Initialise Weights/Biases.
    int iTotalLayers = pModel->iHiddenLayers_ + 2; // One for input, one for output. 
    srand(time(0));
    double*** pWeights =(double***)malloc((iTotalLayers-1)*sizeof(double**));
    for (int i = 0; i < iTotalLayers-1; i++){
        pWeights[i] = (double**)calloc(pTempSizeLayers[i]+1, sizeof(double*));
    }
    for (int i = 0; i < iTotalLayers-1; i++){
        for (int j = 0; j < pTempSizeLayers[i]+1; j++){
            pWeights[i][j] = (double*)calloc(pTempSizeLayers[i+1], sizeof(double));
        }
    }
    for (int i = 0; i < iTotalLayers-1; i++){
        for (int j = 0; j < pTempSizeLayers[i]+1; j++){
            for (int k = 0; k < pTempSizeLayers[i+1]; k++){
                // Between -1,1
                pWeights[i][j][k] = (((double)rand() / ((double)RAND_MAX))*2*2.71)-2.71;
                //pWeights[i][j][k] = ((double)rand() / ((double)RAND_MAX));
            }
        }
    }
    pModel->pWeights_ = pWeights;
    pModel->iIsRegression_ = iIsRegression;
    return pModel;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

double** AllocateLayersOutputCache(MlpClassifier* pModel)
{
    int iTotalLayers = pModel->iHiddenLayers_ + 2;
    double** pArray = (double**)calloc(iTotalLayers, sizeof(double*));
    for (int i = 0; i < iTotalLayers; i++){
        pArray[i] = (double*)calloc(pModel->pLayersSize_[i]+1, sizeof(double));
    }
    return pArray;
}

double** AllocateLayersInputCache(MlpClassifier* pModel)
{
    int iTotalLayers = pModel->iHiddenLayers_ +2 ;
    double** pArray = (double**)calloc(iTotalLayers, sizeof(double*));
    for (int i = 0; i < iTotalLayers; i++){
        pArray[i] = (double*)calloc(pModel->pLayersSize_[i], sizeof(double));
    }
    return pArray;
}

void TrainModel(MlpClassifier* pModel)
{
    if(pModel->iBackpropAlgo_ == 0){
        SGDTrainModel(pModel);
        return;
    }
    int iBatchSize;
    if(pModel->iBackpropAlgo_ == 1){
        iBatchSize = pModel->iMaxTrainIndex_ - pModel->iMinTrainIndex_;
        BatchTrainModel(pModel,iBatchSize);
        return;
    }
    //iBatchSize = pModel->iMiniBatchSize_;
    iBatchSize = 32;
    BatchTrainModel(pModel,iBatchSize);
    return;
}
/*
void SGDTrainModel(MlpClassifier* pModel)
{
    double** pLayersInputCache = AllocateLayersInputCache(pModel); 
    double** pLayersOutputCache = AllocateLayersOutputCache(pModel); 
    int iCount = 0;
    //int iTrainingIndex = pModel->iMinTrainIndex_;  
    
    double* pLossData = (double*)calloc(pModel->iMaxIterations_, sizeof(double));

    for(; iCount < pModel->iMaxIterations_ ; iCount++)
    {
        for(int iTrainingIndex = pModel->iMinTrainIndex_; iTrainingIndex < pModel->iMaxTrainIndex_;
                    iTrainingIndex++)
        {
            Forward_Propogate( pModel, iTrainingIndex, pLayersInputCache, pLayersOutputCache);
        
            double dOutputVal = pLayersOutputCache[pModel->iHiddenLayers_+1][1];
            AddComputeLossMSE(pModel,dOutputVal,pLossData,iTrainingIndex,iCount);
            
            Backward_Propogate(pModel, iTrainingIndex, pLayersInputCache, pLayersOutputCache);
            //iTrainingIndex = (iTrainingIndex+1) % pModel->iMaxTrainIndex_;
        }
    }
    int n = pModel->iMaxTrainIndex_ - pModel->iMinTrainIndex_;
    MeanComputeLossMSE(pModel,pLossData,n);
    //int iTrainingCount = pModel->iMaxTrainIndex_ - pModel->iMinTrainIndex_;    
    WriteLossData(pModel, pLossData);
}
*/
void SGDTrainModel(MlpClassifier* pModel)
{
    double** pLayersInputCache = AllocateLayersInputCache(pModel); 
    double** pLayersOutputCache = AllocateLayersOutputCache(pModel); 

    //Implementing using SGD
    int iCount = 0;
    int iTrainingIndex = pModel->iMinTrainIndex_;  
    // iMinTrainIndex_ is 0. Can add an assert check.
    double* pLossData = (double*)calloc(pModel->iMaxIterations_, sizeof(double));

    for(; iCount < pModel->iMaxIterations_ ; iCount++)
    {
        Forward_Propogate( pModel, iTrainingIndex, pLayersInputCache, pLayersOutputCache);
        double dOutputVal = pLayersOutputCache[pModel->iHiddenLayers_+1][1];
        ComputeLossMSE(pModel,dOutputVal,pLossData,iTrainingIndex,iCount);
        Backward_Propogate(pModel, iTrainingIndex, pLayersInputCache, pLayersOutputCache);
        iTrainingIndex = (iTrainingIndex+1) % pModel->iMaxTrainIndex_;
    }
    //WriteLossData(pModel, pLossData);
}

void BatchTrainModel(MlpClassifier* pModel, int iBatchSize)
{
    srand(time(0));
    double** pLayersInputCache = AllocateLayersInputCache(pModel); 
    double** pLayersOutputCache = AllocateLayersOutputCache(pModel); 

    int iCount = 0;
    int iTrainingIndex = pModel->iMinTrainIndex_;  
    double* pLossData = (double*)calloc(pModel->iMaxIterations_, sizeof(double));

    for(; iCount < pModel->iMaxIterations_ ; iCount++)
    {
        double*** pWeightCorrectionArr = AllocateWeightsCorrectionMatrix(pModel->iHiddenLayers_+2, pModel->pLayersSize_);
        for(int i=0; i<iBatchSize; i++){
            iTrainingIndex = (int) rand() % pModel->iMaxTrainIndex_;
            Forward_Propogate( pModel, iTrainingIndex, pLayersInputCache, pLayersOutputCache);
            
            double dOutputVal = pLayersOutputCache[pModel->iHiddenLayers_+1][1];
            AddComputeLossMSE(pModel,dOutputVal,pLossData,iTrainingIndex,iCount);
            
            Backward_Propogate_Batch(pModel, iTrainingIndex, pLayersInputCache, pLayersOutputCache,pWeightCorrectionArr);            
        }
        AverageWeightCorrections(pModel,pWeightCorrectionArr,iBatchSize);
        ApplyWeightCorrections(pModel, pWeightCorrectionArr);
        FreeWeightCorrectionMatrix(pModel,pWeightCorrectionArr);
    }
    MeanComputeLossMSE(pModel,pLossData,iBatchSize);
    //WriteLossData(pModel, pLossData);
}

void PrintModelWeights(MlpClassifier* pModel)
{
    double*** pTemp = pModel->pWeights_;
    int iTotalLayers = pModel->iHiddenLayers_ + 2;
    int* pTempSizeLayers  = pModel->pLayersSize_;
    for (int i = 0; i < iTotalLayers-1; i++){
        printf("%d : \n",i);
        for (int j = 0; j < pTempSizeLayers[i]+1; j++){
            printf("%d->",j);
            for (int k = 0; k < pTempSizeLayers[i+1]; k++){
                printf("%f ,",pTemp[i][j][k]);
            }
            printf("\n");
        }
        printf("end %d.\n",i);
    }
}

void ClassifyBinaryTestData(MlpClassifier* pModel)
{
    double** pLayersInputCache = AllocateLayersInputCache(pModel); 
    double** pLayersOutputCache = AllocateLayersOutputCache(pModel);
    //int iTestSize = pModel->iMaxTestIndex_ - pModel->iMinTestIndex_ + 1;
    double** pFinalOutputs = (double**)calloc(pModel->iMaxTestIndex_, sizeof(double*));
    for (int i = 0; i < pModel->iMaxTestIndex_; i++){
        pFinalOutputs[i] = (double*)calloc(pModel->iOLSize_, sizeof(double));
    }
    int iTotalTests = 0;
    int iTestPassed = 0;
    int iTestFailed = 0;
    int iTestFP = 0;
    int iTestFN = 0;
    int iTestTP = 0;
    int iTestTN = 0;
    for(int iTestIndex = pModel->iMinTestIndex_; iTestIndex <= pModel->iMaxTestIndex_; iTestIndex++){
        printf("Testing For Sample : %d ->\n", iTestIndex);
        // for(int i=0; i<pModel->iNumberOfFeatures_; i++){
        //     printf("%f,", pModel->pData_[iTestIndex][i]);
        // }
        Forward_Propogate(pModel, iTestIndex, pLayersInputCache, pLayersOutputCache);
        int iExpectedValue = 0;
        double dExpectedValue = pModel->pData_[iTestIndex][0];
        if(dExpectedValue > 0.5){
            iExpectedValue = 1;
        }
        double dTempOutput =  pLayersOutputCache[pModel->iHiddenLayers_+1][1];
        int iPredictedValue = 0;
        if(dTempOutput > 0.5){
            iPredictedValue = 1;
        }
        printf("Expected Classification = %d\n",iExpectedValue);
        printf("Predicated Output = %f,  Classification=%d\n", dTempOutput,iPredictedValue);
        if(iPredictedValue==iExpectedValue){
            iTestPassed++;
            if(iPredictedValue==1) iTestTP++;
            else iTestTN++;
        }else{
            iTestFailed++;
            if(iExpectedValue==0) iTestFN++;
            else iTestFP++;
        }
        iTotalTests++;
    }
    printf("Summary :: \n");
    printf("TruePositive = %d, TrueNegative=%d, FalsePositive=%d, FalseNegative=%d\n"
            , iTestTP, iTestTN, iTestFP, iTestFN);
    printf("Accuracy = %f%%\n", (((iTestPassed*1.0))/iTotalTests)*100);
}


void ClassifyRegressionTestData(MlpClassifier* pModel)
{
    double** pLayersInputCache = AllocateLayersInputCache(pModel); 
    double** pLayersOutputCache = AllocateLayersOutputCache(pModel);
    //int iTestSize = pModel->iMaxTestIndex_ - pModel->iMinTestIndex_ + 1;
    // double** pFinalOutputs = (double**)calloc(pModel->iMaxTestIndex_, sizeof(double*));
    // for (int i = 0; i < pModel->iMaxTestIndex_; i++){
    //     pFinalOutputs[i] = (double*)calloc(pModel->iOLSize_, sizeof(double));
    // }
    // for (int i = 0; i < pModel->iMaxTestIndex_; i++){
    //     pFinalOutputs[i] = (double*)calloc(pModel->iOLSize_, sizeof(double));
    // }
    double dSum = 0;
    int n = 1;
    for(int iTestIndex = pModel->iMinTestIndex_; iTestIndex <= pModel->iMaxTestIndex_; iTestIndex++){
        printf("Testing For Sample : %d ->\n", iTestIndex);
        // for(int i=0; i<pModel->iNumberOfFeatures_; i++){
        //     printf("%f,", pModel->pData_[iTestIndex][i]);
        // }
        Forward_Propogate(pModel, iTestIndex, pLayersInputCache, pLayersOutputCache);
        //printf("\nOutput Result = %f\n", pLayersOutputCache[pModel->iHiddenLayers_+1][1]);
        double dExpectedValue = pModel->pData_[iTestIndex][0];
        double dPredictedValue = pLayersOutputCache[pModel->iHiddenLayers_+1][1];
        dSum += (dExpectedValue - dPredictedValue)*(dExpectedValue - dPredictedValue); 
        n++;
        printf("Expected Value = %f,  Predicted Value = %f\n", dExpectedValue, dPredictedValue);
        printf("\n");
    }
    printf("MSE value on test data = %f" , dSum/(2*n));

    //How to print accuracy??
}

void ClassifyData(MlpClassifier* pModel){
    if(pModel->iIsRegression_ == 1){
        ClassifyRegressionTestData(pModel);
    }else{
        ClassifyBinaryTestData(pModel);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void Forward_Propogate(MlpClassifier* pModel, int iTrainingIndex, 
    double** pLayersInputCache, double** pLayersOutputCache) 
{
    int iInputFeatures = pModel->iNumberOfFeatures_ -1 ; 
    // (Subtracted. the Output Feature)
    for (int i = 0; i < iInputFeatures; i++){
        pLayersInputCache[0][i] = pModel->pData_[iTrainingIndex][i+1];
        pLayersOutputCache[0][i+1] = pModel->pData_[iTrainingIndex][i+1];
    }
    pLayersOutputCache[0][0] = 1; // Bias Input -> 1. 


    int iTotalLayers = pModel->iHiddenLayers_ + 2;
    for (int i = 1; i < iTotalLayers ; i++) {
        // Compute (i)th layer activation function inputs. from (i-1)th layer outputs.
        MatrixMultiply(pLayersOutputCache[i-1], pModel->pWeights_[i-1], pLayersInputCache[i],
            pModel->pLayersSize_[i-1]+1, pModel->pLayersSize_[i]);

        if(i == iTotalLayers-1 ){
            // Output Layer. No Activation function in case of Regression.
            if(pModel->iIsRegression_ == 1){
                pLayersOutputCache[i][0] = 1; 
                int iSize = pModel->pLayersSize_[i];
                for (int j = 0; j < iSize; j++) {
                    pLayersOutputCache[i][j+1] = pLayersInputCache[i][j];
                }
                continue;
            }
            // Softmax Activation function in case of Cross Entropy.
            if(pModel->iCostFunction_ == 1){
                SoftmaxFunc(pModel->pLayersSize_[i],pLayersInputCache[i],pLayersOutputCache[i]);
                continue;
            }
        }
        // Compute layer_outputs[i] using activation functions.
        switch (pModel->iActivationFunction_) {
            case 0: 
                SigmoidFunc(pModel->pLayersSize_[i], pLayersInputCache[i], pLayersOutputCache[i]);
                break;
            case 1: 
                TanhFunc(pModel->pLayersSize_[i], pLayersInputCache[i], pLayersOutputCache[i]);
                break;
            case 2:
                ReLuFunc(pModel->pLayersSize_[i], pLayersInputCache[i], pLayersOutputCache[i]);
                break;
            default:
                SigmoidFunc(pModel->pLayersSize_[i], pLayersInputCache[i], pLayersOutputCache[i]);
                break;
        }
    }
}

void MatrixMultiply(double* pInputArr, double** pWeightsArr, double* pOuputArr, int iInputSize, int iOutputSize) {
    // pInputArr : [iInputSize]
    // pOuputArr : [iOutputSize]
    // pWeightsArr : [iInputSize] x [iOutputSize]
    for (int o = 0; o < iOutputSize; o++) {
        pOuputArr[o] = 0.0;
        for (int i=0; i<iInputSize; i++){
            pOuputArr[o] += (pInputArr[i] * pWeightsArr[i][o]);
        }
    }
}

void SigmoidFunc(int iSize, double* pInputArr, double* pOuputArr) {
    pOuputArr[0] = 1; 
    for (int i = 0; i < iSize; i++) {
        pOuputArr[i+1] = (1.0) / (1.0 + exp(-pInputArr[i]));
    }
}

void TanhFunc(int iSize, double* pInputArr, double* pOuputArr) {
    pOuputArr[0] = 1;
    for (int i = 0; i < iSize; i++) {
        pOuputArr[i+1] = tanh(pInputArr[i]);
    }
}

void ReLuFunc(int iSize, double* pInputArr, double* pOuputArr) {
    pOuputArr[0] = 1; 
    for (int i = 0; i < iSize; i++) {
        //pOuputArr[i+1] = max(pInputArr[i],0.0);
        pOuputArr[i+1] = fmax(pInputArr[i],0.0);
    }
}
void SoftmaxFunc(int iSize, double* pInputArr, double* pOuputArr) {
    //Should not be used in regression.
    pOuputArr[0] = 1; // Bias term
    double dSum = 0.0;
    // if(iSize == 1){
    //     //Binary Classification or Regression
    //     double d1 = exp(pInputArr[0]);
    //     double d2 = exp(1-pInputArr[0]);
    //     dSum = d1 + d2;
    //     pOuputArr[1] = d1/dSum;
    //     return;
    // }
    // This doesnt seem right.
    for(int i = 0; i < iSize; i++){
        dSum += exp(pInputArr[i]);
    }
    for(int i = 0; i < iSize; i++) {
        pOuputArr[i+1] = exp(pInputArr[i]) / dSum;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////


void DerivativeSigmoid(int iLayerSize, double* pLayerOutput, double* pLayerDerivativeArr) {
    // d(sigmoid(x))/d(x) = sigmoid(x) * (1-sigmoid(x))
    for (int i = 0; i < iLayerSize; i++){
        pLayerDerivativeArr[i] = pLayerOutput[i+1] * (1.0 - pLayerOutput[i+1]);
    }
}

void DerivativeTanh(int iLayerSize, double* pLayerOutput, double* pLayerDerivativeArr) {
    // d(tanh (x))/d(x) = 1 - (tanh (x))^2
    for (int i = 0; i < iLayerSize; i++){
        pLayerDerivativeArr[i] = 1.0 - (pLayerOutput[i+1] * pLayerOutput[i+1]);
    }
}

void DerivativeReLu(int iLayerSize, double* pLayerInput, double* pLayerDerivativeArr) {
    for (int i = 0; i < iLayerSize; i++) {
        if (pLayerInput[i] > 0){
            pLayerDerivativeArr[i] = 1;
        }
        else if (pLayerInput[i] < 0){
            pLayerDerivativeArr[i] = 0;
        }
        else{
             // derivative does not exist
            pLayerDerivativeArr[i] = 0.5; // giving arbitrary value
        }
    }
}

void DerivativeSoftmax(int iLayerSize, double* pLayerOutput, double* pLayerDerivativeArr) {
    for (int i = 0; i < iLayerSize; i++){
        pLayerDerivativeArr[i] = pLayerOutput[i+1] * (1.0 - pLayerOutput[i+1]);
    }
}

double*** AllocateWeightsCorrectionMatrix(int iTotalLayers, int* pLayerSizes)
{
    double*** pWeightCorrectionArr = (double***)calloc(iTotalLayers-1, sizeof(double**));

    for (int i = 0; i < iTotalLayers-1; i++){
        pWeightCorrectionArr[i] = (double**)calloc(pLayerSizes[i]+1, sizeof(double*));
    }

    for (int i = 0; i < iTotalLayers-1; i++){
        for (int j = 0; j < pLayerSizes[i]+1; j++){
            pWeightCorrectionArr[i][j] = (double*)calloc(pLayerSizes[i+1], sizeof(double));
        }
    }
    return pWeightCorrectionArr;
}

double* AllocateExpectedOutputArray(MlpClassifier* pModel, int iDataIndex)
{
    int iTotalLayers = pModel->iHiddenLayers_ + 2;
    int iOutputLayerSize = pModel->pLayersSize_[iTotalLayers - 1];
    double* pExpectedOutputArr = (double*)calloc(iOutputLayerSize, sizeof(double));
    if (iOutputLayerSize == 1){
        pExpectedOutputArr[0] = pModel->pData_[iDataIndex][0];
    }else {
        pExpectedOutputArr[(int)(pModel->pData_[iDataIndex][0] - 1)] = 1;
    }
    return pExpectedOutputArr;
}

double** AllocateGradientMatrix(MlpClassifier* pModel){
    double** pArray = (double**)calloc(pModel->iHiddenLayers_+2, sizeof(double*));
    for (int i = 0; i < pModel->iHiddenLayers_+2; i++){
        pArray[i] = (double*)calloc(pModel->pLayersSize_[i], sizeof(double));
    }
    return pArray;
}

void Backward_Propogate(MlpClassifier* pModel, int iTrainingDataIndex, 
    double** pLayersInputCache, double** pLayersOutputCache) 
{
    double* pExpectedOutputArr = AllocateExpectedOutputArray(pModel, iTrainingDataIndex);
    // This array can also be an array of just one element. 
    
    double*** pWeightCorrectionArr = AllocateWeightsCorrectionMatrix(pModel->iHiddenLayers_+2, pModel->pLayersSize_);
    double** pNodesGradientArr = AllocateGradientMatrix(pModel);
    
    ComputeGradientsForLastLayer(pModel,pNodesGradientArr,pExpectedOutputArr,pLayersInputCache, pLayersOutputCache);
    ComputeWeightCorrectionForLastLayer(pModel,pWeightCorrectionArr,pNodesGradientArr,pLayersOutputCache);
    ComputeGradientsForHiddenLayers(pModel,pNodesGradientArr,pExpectedOutputArr,pLayersInputCache,pLayersOutputCache);
    ComputeWeightCorrectionForHiddenLayers(pModel,pWeightCorrectionArr,pNodesGradientArr,pLayersOutputCache);
    ApplyWeightCorrections(pModel, pWeightCorrectionArr);

    FreeGradientMatrix(pModel, pNodesGradientArr);
    FreeWeightCorrectionMatrix(pModel,pWeightCorrectionArr);
    free(pExpectedOutputArr);
    return;
}

void Backward_Propogate_Batch(MlpClassifier* pModel, int iTrainingDataIndex, 
    double** pLayersInputCache, double** pLayersOutputCache, double*** pWeightCorrectionArr) 
{
    double* pExpectedOutputArr = AllocateExpectedOutputArray(pModel, iTrainingDataIndex);
    //printf("Expected : %f\n", pExpectedOutputArr[0]);
    // This array can also be an array of just one element. 
    //PrintExpectedOutput(pExpectedOutputArr,pModel);
    //double*** pWeightCorrectionArr = AllocateWeightsCorrectionMatrix(pModel->iHiddenLayers_+2, pModel->pLayersSize_);
    double** pNodesGradientArr = AllocateGradientMatrix(pModel);
    ComputeGradientsForLastLayer(pModel,pNodesGradientArr,pExpectedOutputArr,pLayersInputCache, pLayersOutputCache);
    AddWeightCorrectionForLastLayer(pModel,pWeightCorrectionArr,pNodesGradientArr,pLayersOutputCache);
    ComputeGradientsForHiddenLayers(pModel,pNodesGradientArr,pExpectedOutputArr,pLayersInputCache,pLayersOutputCache);
    AddWeightCorrectionForHiddenLayers(pModel,pWeightCorrectionArr,pNodesGradientArr,pLayersOutputCache);
    //ApplyWeightCorrections(pModel, pWeightCorrectionArr);

    FreeGradientMatrix(pModel, pNodesGradientArr);
    //FreeWeightCorrectionMatrix(pModel,pWeightCorrectionArr);
    free(pExpectedOutputArr);
    return;
}


void ComputeGradientsForLastLayer(MlpClassifier* pModel,double** pNodesGradientArr,double* pExpectOutputArr, double** pLayersInputCache,
                double** pLayersOutputCache)
{
    int iTotalLayers = pModel->iHiddenLayers_ + 2;
    double** pLayersDerivatives = (double**)calloc(iTotalLayers, sizeof(double*));
    for (int i = 0; i < iTotalLayers; i++){
        pLayersDerivatives[i] = (double*)calloc(pModel->pLayersSize_[i], sizeof(double));
    }
    // Error produced at the output layer.
    // Add logic for if/else for different error calculations.
    double* pErrorOutput = (double*)calloc(pModel->iOLSize_, sizeof(double));

    for (int i = 0; i < pModel->iOLSize_; i++){
        if(pModel->iCostFunction_==1){
            // Cross Entropy Cost function derivative. 
            // Need to correct it. 
            pErrorOutput[i] =  pLayersOutputCache[iTotalLayers-1][i+1] - pExpectOutputArr[i];
        }else{
            // MSE Cost function derivative.
            pErrorOutput[i] =  - pExpectOutputArr[i] + pLayersOutputCache[iTotalLayers-1][i+1];
        }
    }

    int iOutputLayerIndex = iTotalLayers-1;
    // Softmax was the activation function in case of Cross Entropy. 
    if(pModel->iCostFunction_ == 1){
        DerivativeSoftmax(pModel->iOLSize_, pLayersOutputCache[iOutputLayerIndex], pLayersDerivatives[iOutputLayerIndex]);
        for (int i = 0; i < pModel->iOLSize_; i++){
            pNodesGradientArr[iOutputLayerIndex][i] = pErrorOutput[i]* pLayersDerivatives[iOutputLayerIndex][i];
        }
        return;
    }

    // No Activation function in case of regression. 
    if(pModel->iIsRegression_ == 1){
        for (int i = 0; i < pModel->iOLSize_; i++){
            pNodesGradientArr[iOutputLayerIndex][i] = pErrorOutput[i];
        }
    }else{
        switch(pModel->iActivationFunction_) {
        case 0: 
            DerivativeSigmoid(pModel->iOLSize_, pLayersOutputCache[iOutputLayerIndex], pLayersDerivatives[iOutputLayerIndex]);
            for (int i = 0; i < pModel->iOLSize_; i++){
                pNodesGradientArr[iOutputLayerIndex][i] = pErrorOutput[i] * pLayersDerivatives[iOutputLayerIndex][i];
            }
            break;
        case 1: 
            DerivativeTanh(pModel->iOLSize_,  pLayersOutputCache[iOutputLayerIndex], pLayersDerivatives[iOutputLayerIndex]);
            for (int i = 0; i < pModel->iOLSize_; i++){
                pNodesGradientArr[iOutputLayerIndex][i] = pErrorOutput[i] * pLayersDerivatives[iOutputLayerIndex][i];
            }
            break;
        case 2:
            DerivativeReLu(pModel->iOLSize_, pLayersInputCache[iOutputLayerIndex], pLayersDerivatives[iOutputLayerIndex]);
            for (int i = 0; i < pModel->iOLSize_; i++){
                pNodesGradientArr[iOutputLayerIndex][i] = pErrorOutput[i] * pLayersDerivatives[iOutputLayerIndex][i];
            }
            break;
        default:
            DerivativeSigmoid(pModel->iOLSize_, pLayersOutputCache[iOutputLayerIndex], pLayersDerivatives[iOutputLayerIndex]);
            for (int i = 0; i < pModel->iOLSize_; i++){
                pNodesGradientArr[iOutputLayerIndex][i] = pErrorOutput[i] * pLayersDerivatives[iOutputLayerIndex][i];
            }
            break;
        }
    }
    // Free Memory Not Needed.
    free(pErrorOutput);
    for (int i = 0; i < iTotalLayers; i++){
        free(pLayersDerivatives[i]);
    }
    free(pLayersDerivatives);
    return ;
}

void ComputeWeightCorrectionForLastLayer(MlpClassifier* pModel,double*** pWeightCorrectionArr,double** pNodesGradientArr,
                    double** pLayersOutputCache){
    int iLastHiddenLayerIndex = (pModel->iHiddenLayers_ + 2) - 2;
    int iLastWeightArrayIndex = iLastHiddenLayerIndex; // Since there are (TotalLayers - 1) weight arrays.
    int iOutputLayerIndex = iLastHiddenLayerIndex + 1;
    // (pModel->iHiddenLayers_ + 2) = TotalLayers including input and output layer.
    for (int i = 0; i < pModel->iOLSize_; i++){
        for (int j = 0; j < pModel->pLayersSize_[iLastHiddenLayerIndex]+1; j++){
            pWeightCorrectionArr[iLastWeightArrayIndex][j][i] = pModel->dLearningRate_
                                        * pNodesGradientArr[iOutputLayerIndex][i] * pLayersOutputCache[iLastHiddenLayerIndex][j];
        }
    }
}

void AddWeightCorrectionForLastLayer(MlpClassifier* pModel,double*** pWeightCorrectionArr,double** pNodesGradientArr,
                    double** pLayersOutputCache){
    int iLastHiddenLayerIndex = (pModel->iHiddenLayers_ + 2) - 2;
    int iLastWeightArrayIndex = iLastHiddenLayerIndex; // Since there are (TotalLayers - 1) weight arrays.
    int iOutputLayerIndex = iLastHiddenLayerIndex + 1;
    // (pModel->iHiddenLayers_ + 2) = TotalLayers including input and output layer.
    for (int i = 0; i < pModel->iOLSize_; i++){
        for (int j = 0; j < pModel->pLayersSize_[iLastHiddenLayerIndex]+1; j++){
            pWeightCorrectionArr[iLastWeightArrayIndex][j][i] += pModel->dLearningRate_
                                        * pNodesGradientArr[iOutputLayerIndex][i] * pLayersOutputCache[iLastHiddenLayerIndex][j];
        }
    }
}

void ComputeGradientsForHiddenLayers(MlpClassifier* pModel,double** pNodesGradientArr,double* pExpectOutputArr, double** pLayersInputCache,
                double** pLayersOutputCache)
{
    int iTotalLayers = pModel->iHiddenLayers_ + 2;
    double** pLayersDerivatives = (double**)calloc(iTotalLayers, sizeof(double*));
    for (int i = 0; i < iTotalLayers; i++){
        pLayersDerivatives[i] = (double*)calloc(pModel->pLayersSize_[i], sizeof(double));
    }
    for (int iLayerIndex = pModel->iHiddenLayers_ ; iLayerIndex >= 1; iLayerIndex--){
        switch(pModel->iActivationFunction_) {
            case 0: 
                DerivativeSigmoid(pModel->pLayersSize_[iLayerIndex], pLayersOutputCache[iLayerIndex], pLayersDerivatives[iLayerIndex]);
                break;
            case 1: 
                DerivativeTanh(pModel->pLayersSize_[iLayerIndex], pLayersOutputCache[iLayerIndex], pLayersDerivatives[iLayerIndex]);
                break;
            case 2:
                DerivativeReLu(pModel->pLayersSize_[iLayerIndex], pLayersInputCache[iLayerIndex], pLayersDerivatives[iLayerIndex]);
                break;
            default : 
                DerivativeSigmoid(pModel->pLayersSize_[iLayerIndex], pLayersOutputCache[iLayerIndex], pLayersDerivatives[iLayerIndex]);
                break;
        }
        for (int i = 0; i < pModel->pLayersSize_[iLayerIndex]; i++){
            double dError = 0.0;
            for(int j=0; j < pModel->pLayersSize_[iLayerIndex+1]; j++){
                dError += pNodesGradientArr[iLayerIndex+1][j] * pModel->pWeights_[iLayerIndex][i][j];    
            }
            pNodesGradientArr[iLayerIndex][i] = dError * pLayersDerivatives[iLayerIndex][i];
        }
    }
}

void ComputeWeightCorrectionForHiddenLayers(MlpClassifier* pModel,double*** pWeightCorrectionArr,double** pNodesGradientArr,
                    double** pLayersOutputCache)
{
    for (int i = pModel->iHiddenLayers_; i >= 1; i--) {
        for (int j = 0; j < pModel->pLayersSize_[i]; j++) {
            for (int k = 0; k < pModel->pLayersSize_[i-1]+1; k++){
                pWeightCorrectionArr[i-1][k][j] = pModel->dLearningRate_ * pNodesGradientArr[i][j] * pLayersOutputCache[i-1][k];
            }
        }
    }
}

void AddWeightCorrectionForHiddenLayers(MlpClassifier* pModel,double*** pWeightCorrectionArr,double** pNodesGradientArr,
                    double** pLayersOutputCache)
{
    for (int i = pModel->iHiddenLayers_; i >= 1; i--) {
        for (int j = 0; j < pModel->pLayersSize_[i]; j++) {
            for (int k = 0; k < pModel->pLayersSize_[i-1]+1; k++){
                pWeightCorrectionArr[i-1][k][j] += pModel->dLearningRate_ * pNodesGradientArr[i][j] * pLayersOutputCache[i-1][k];
            }
        }
    }
}

void AverageWeightCorrections(MlpClassifier* pModel,double*** pWeightCorrectionArr,int iBatchSize)
{
    int iTotalLayers = pModel->iHiddenLayers_ +2;
    for (int i = 0; i < iTotalLayers-1; i++) {
        for (int j = 0; j < pModel->pLayersSize_[i]+1; j++) {
            for (int k = 0; k < pModel->pLayersSize_[i+1]; k++) {
                pWeightCorrectionArr[i][j][k] = pWeightCorrectionArr[i][j][k] / iBatchSize;
            }
        }
    }
}

void ApplyWeightCorrections(MlpClassifier* pModel, double*** pWeightCorrectionArr){
    int iTotalLayers = pModel->iHiddenLayers_ +2;
    for (int i = 0; i < iTotalLayers-1; i++) {
        for (int j = 0; j < pModel->pLayersSize_[i]+1; j++) {
            for (int k = 0; k < pModel->pLayersSize_[i+1]; k++) {
                pModel->pWeights_[i][j][k] -= pWeightCorrectionArr[i][j][k];
            }
        }
    }
}



void FreeGradientMatrix(MlpClassifier* pModel, double** pNodesGradientArr){
    for (int i = 0; i < pModel->iHiddenLayers_+2; i++){
        free(pNodesGradientArr[i]);
    }
    free(pNodesGradientArr);
}


void FreeWeightCorrectionMatrix(MlpClassifier* pModel,double*** pWeightCorrectionArr){
    int iTotalLayers = pModel->iHiddenLayers_ +2;
    for (int i = 0; i < iTotalLayers - 1; i++){
        for (int j = 0; j < pModel->pLayersSize_[i]+1; j++){
            free(pWeightCorrectionArr[i][j]);
        }
    }
    for (int i= 0; i < iTotalLayers - 1; i++){
        free(pWeightCorrectionArr[i]);
    }
    free(pWeightCorrectionArr);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void parseSingleLine(char* pLine, int iNumOfFeatures, double** pData, int iIndex)
{
    //printf("Line=%s\n",pLine);
    char* pToken = strtok(pLine, ",");
    int iCount = 0;
    while(pToken!=NULL && pToken[0]!='\0') {
    //while(pToken!=NULL) {
        pData[iIndex][iCount++] = atof(pToken);
        pToken = strtok(NULL, ",\n");
    }
    return;
}


int ReadInputData(char* pFileName, int iNumOfRows, int iNumOfFeatures, double** pData)
{
    FILE* pFileHandle = fopen(pFileName, "r");
    if(pFileHandle == NULL){
        printf("Could not open file '%s'.\n", pFileName);
        return -1;
    }
    int iMaxLineSize = 1024*1024;
    char* pLine = (char*)malloc(iMaxLineSize * sizeof(char));
    for(int i=0; i<iNumOfRows; i++){
        if(fgets(pLine, iMaxLineSize, pFileHandle)){
            //Parse Features. 
            parseSingleLine(pLine,iNumOfFeatures,pData,i);
        }else{
            //ERROR. 
            printf("Could not parse expected(%d) rows of data in file %s. Only %d rows found."
                    , iNumOfRows, pFileName, i);
            return -1;
        }
    }
    //We do not care for lines more than iNumOfRows.
    free(pLine);
    fclose(pFileHandle);
    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////


void PrintInputOutputCache(MlpClassifier* pModel,double** pArrOut, double** pArrIn)
{
    printf("InputsCache : \n");
    for(int i=0; i<pModel->iHiddenLayers_+2; i++){
        for(int j=0; j<pModel->pLayersSize_[i]; j++){
            printf("%f,",pArrIn[i][j]);
        }
        printf("\n");
    }
    printf("OutputsCache: \n");
    for(int i=0; i<pModel->iHiddenLayers_+2; i++){
        for(int j=0; j<pModel->pLayersSize_[i]+1; j++){
            printf("%f,",pArrOut[i][j]);
        }
        printf("\n");
    }
}
