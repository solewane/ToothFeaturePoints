'''
Written by TangRenjie in 2017.7

Usage:
python svr.py
'''

import time
import numpy as np
import pickle
import os
from sklearn.svm import SVR

# Define

feature_list = []
file_list = os.listdir('E:\\data\\feature_points')

xml_file = []
with open('..\\extract\\all.xml', 'r', encoding='UTF-8') as inFile:
    lines = inFile.readlines()
for single_line in lines:
    start_pos = single_line.find('\" name=\"') + 8
    end_pos = single_line.find('\" is')
    if start_pos != -1 and end_pos != -1:
        xml_file.append(single_line[start_pos:end_pos])

for file_name in file_list:
    with open('E:\\data\\feature_points\\' + file_name, 'r') as inFile:
        file_lines = inFile.readlines()
    for single_line in file_lines:
        end_pos = single_line.find(':')
        tmp = single_line[0:end_pos]
        if (tmp not in feature_list) and (tmp in xml_file):
            feature_list.append(tmp)

# print(xml_file)
print(feature_list)

for feature in feature_list:
    print(feature)

    file_path = 'E:\\trainset'

    feature_path = file_path + '\\' + feature + '.txt'

    # Input and Output
    X = []
    y = []
    w = []
    a = 0
    b = 0
    c = 0
    with open(feature_path, 'r') as inFile:
        file_list = inFile.readlines()
    for i in file_list:
        dataList = list(map(eval, i.rstrip('\n').rstrip(' ').split(' ')))
        y.append(dataList[0])
        if dataList[0] == 1:
            w.append(1000)
        else:
            if dataList[0] == 0.5:
                w.append(100)
            else:
                w.append(1)
        X.append(dataList[1:])

    print('Read complete.')

    # SVR
    svr = SVR(kernel='rbf', C=1e3, gamma=1)

    t0 = time.time()

    svr.fit(X, y, sample_weight = w)

    train_time = time.time() - t0

    print("Train Finished. Time: %d s" % train_time)

    with open(feature + '.dat', 'wb') as f:
        pickle.dump(svr, f)
