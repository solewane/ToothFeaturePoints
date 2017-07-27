'''
Written by TangRenjie in 2017.7

Usage:
python predict.py inputFile featureName outDir
'''

import pickle
import time
import sys

in_path = sys.argv[1]
feature = sys.argv[2]
out_path = sys.argv[3] + feature + '_out.txt'
model_path = 'E:\\repo\\ToothFeaturePoints\\train\\'

with open(model_path + feature + '.dat', 'rb') as f:
    svr = pickle.load(f)

with open(in_path, 'r') as f:
    a = f.readlines()

z = []
id_list = []
for i in a:
    line = list(map(eval, i.rstrip('\n').rstrip(' ').split(' ')))
    id_list.append(line[0])
    z.append(line[1:])

#predict
t0 = time.time()

predict_result = svr.predict(z)

predict_time = time.time() - t0

print("Predict Finished. Time: %d s" % predict_time)

#output
output = open(out_path, 'w')
for i in range(0, len(predict_result)):
    output.write(str(id_list[i]) + ' ' + str(predict_result[i]) + '\n')
output.close()
