import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPClassifier
from sklearn.metrics import confusion_matrix
from sklearn.neural_network import MLPRegressor
import matplotlib.pyplot as plt


def getMSE(arr1, arr2):
	sum = 0.0
	n = len(arr1)
	i = 0
	while i < n :
		sum += (arr1[i]-arr2[i])*(arr1[i]-arr2[i])
		i = i+1
	sum = sum/2
	sum = sum/n
	return sum



data = pd.read_csv("../data/housing.csv")
ndata=(data-data.mean())/data.std()
ndata['Price'] = data['Price']
data = ndata
# Normalise Data.

training_set, validation_set = train_test_split(data, test_size = 0.2)

X_train = training_set.loc[:, training_set.columns != 'Price']
Y_train = training_set.loc[:, training_set.columns == 'Price']
X_val = validation_set.loc[:, validation_set.columns != 'Price']
Y_val = validation_set.loc[:, validation_set.columns == 'Price']


regr = MLPRegressor(hidden_layer_sizes=(50,50,50), max_iter=100,activation = 'logistic',solver='sgd',random_state=1)

regr.fit(X_train, Y_train.values.ravel())
y_pred = regr.predict(X_val)
y_exp = Y_val.values.ravel()
i = 0
while i < len(y_exp) :
	print("Testing for Sample {}->\nExpected Value = {} , Predicted Value = {}".format(i,y_exp[i],y_pred[i]))
	i = i+1
print("MSE value on predictions = {}".format(getMSE(y_exp,y_pred)))

loss_values = regr.loss_curve_
plt.plot(loss_values)
plt.show()
