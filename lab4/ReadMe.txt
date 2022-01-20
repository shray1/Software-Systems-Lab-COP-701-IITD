-Lab Report is available under the "docs" folder. 

*Please Adhere to the following points to run the MLP-Classifier. 

1. $ make 
   This will build the library and the MLP binary in the "bin" folder. 

2. This classifier takes input data as only real numbers. 
   User should not give any other data in the input file.

3. The first column of data should represent the Output Feature [aka the classification result.] The Ouput Feature should only consists of Integers starting 0. 
	For Binary classification, user may use "0" to represent one class and "1" to represent other class.

4. For regression, the ouput feature may contain real values. But user should specifically mention the size of output layer as only 1, in the command line argument to the MLP binary.

5. User may run the MLP binary without any arguments to see the usage of the binary. 



Sample Commands : 

./bin/mlp  0 0 0 3 50 0.05 10000 1 ./data/normalised.txt 31  500 0.8 0
./bin/mlp  0 0 0 3 50 0.05 10000 1 ./data/normalisedHousing.txt 13  500 0.8 1

<Refer Usage to see what the arguments represent here.> 