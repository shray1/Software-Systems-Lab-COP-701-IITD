import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.neural_network import MLPClassifier
from sklearn.metrics import confusion_matrix
import matplotlib.pyplot as plt

def convertDiagnosis(value):
	if value == 'M':
		return 1
	if value == 'B':
		return 0
	return 3;

def accuracy(cm):
	return (cm[0][0]+cm[1][1])/(cm[0][0] + cm[0][1] + cm[1][0] + cm[1][1])

data = pd.read_csv("../data/data.csv")
data = data.drop('id',axis=1)
data['diagnosis'] = data['diagnosis'].apply(convertDiagnosis)
ndata=(data-data.mean())/data.std()
ndata['diagnosis'] = data['diagnosis']
data = ndata 

training_set, validation_set = train_test_split(data, test_size = 0.2)


# X_train = training_set.iloc[:,0:-1].values
# Y_train = training_set.iloc[:,-1].values
# X_val = validation_set.iloc[:,0:-1].values
# y_val = validation_set.iloc[:,-1].values

X_train = training_set.loc[:, training_set.columns != 'diagnosis']
Y_train = training_set.loc[:, training_set.columns == 'diagnosis']
X_val = validation_set.loc[:, validation_set.columns != 'diagnosis']
Y_val = validation_set.loc[:, validation_set.columns == 'diagnosis']


classifier = MLPClassifier(hidden_layer_sizes=(50,50,50), max_iter=100,activation = 'logistic',solver='sgd',random_state=1)
classifier.fit(X_train, Y_train.values.ravel())
y_pred = classifier.predict(X_val)
cm = confusion_matrix(y_pred, Y_val.values.ravel())
print("Accuracy of MLPClassifier : {}".format(accuracy(cm)))

loss_values = classifier.loss_curve_
plt.plot(loss_values)
plt.show()
