'''
Written by TangRenjie in 2017.7

Usage:
python svr.py
files:
train_in.txt & train_out.txt : train set
data.txt : data set
predict.txt : output the predict
put these file in the same foldier
'''

import time
import numpy as np
from sklearn.svm import SVR

# Input
X = []
with open('train_in.txt', 'r') as inFile:
    file_list = inFile.readlines()
for i in file_list:
    X.append(list(map(eval, i.rstrip('\n').split(' '))))

y = []
with open('train_out.txt', 'r') as inFile:
    file_list = inFile.readlines()
y = list(map(eval, file_list))

z = []
with open('data.txt', 'r') as inFile:
    file_list = inFile.readlines()
for i in file_list:
    z.append(list(map(eval, i.rstrip('\n').split(' '))))

# SVR
svr = SVR(kernel = "linear", C = 1.0, epsilon = 0.01)

t0 = time.time()

svr.fit(X, y)

train_time = time.time() - t0

print("Train Finished. Time: %d s" % train_time)

#predict
t0 = time.time()

predict_result = svr.predict(z)

predict_time = time.time() - t0

print("Predict Finished. Time: %d s" % predict_time)

#output
output = open('predict.txt', 'w')
for i in predict_result:
    output.write(str(i) + "\n")
output.close()
