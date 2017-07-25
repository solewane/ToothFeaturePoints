# coding: utf-8
import os
import math

# Define
model_dir = 'E:\\data\\stl'
file_dir = 'E:\\data\\feature_points'
feature_dir = 'E:\\data\\pos_feature'

file_list = os.listdir(file_dir)
feature_list = []

xml_file = []
with open('all.xml', 'r', encoding='UTF-8') as inFile:
    lines = inFile.readlines()
for single_line in lines:
    start_pos = single_line.find('\" name=\"') + 8
    end_pos = single_line.find('\" is')
    if start_pos != -1 and end_pos != -1:
        xml_file.append(single_line[start_pos:end_pos])

for file_name in file_list:
    with open(file_dir + '\\' + file_name, 'r') as inFile:
        file_lines = inFile.readlines()
    for single_line in file_lines:
        end_pos = single_line.find(':')
        tmp = single_line[0:end_pos]
        if (tmp not in feature_list) and (tmp in xml_file):
            feature_list.append(tmp)

# print(xml_file)

for feature in feature_list:
    if feature[0] != '1':
        break

    output_dir = 'E:\\data\\' + feature
    feature_path = feature_dir + '\\' + feature + '.txt'
     # Get Distribution Graph
    err_small = 0
    err_big = 0

    point_cloud = []
    center = []
    sigma3 = []

    for file_name in file_list:
        try:
            with open(output_dir + '\\' + file_name, 'r') as inFile:
                file_lines = inFile.readlines()
            for single_line in file_lines:
                a = float(list(map(eval, single_line.rstrip('\n').split(' ')))[0])
                b = float(list(map(eval, single_line.rstrip('\n').split(' ')))[1])
                c = float(list(map(eval, single_line.rstrip('\n').split(' ')))[2])
                if a > 0 and a < 1 and b > 0 and b < 1 and c > 0 and c < 1:
                    point_cloud.append([a, b, c])
        except:
            print('Open file err! ' + output_dir + '\\' + file_name)
    
    for i in [0, 1, 2]:
        result_list = []

        # Delete ridicular points
        tmp = []
        for j in point_cloud:
            tmp.append(j[i])
        avg = sum(tmp) / len(tmp)

        for j in point_cloud:
            if (j[i] - avg > -0.2) and (j[i] - avg < 0.2):
                result_list.append(j[i])

        # Calcuate Gaussian Distribution
        n = len(result_list)
        mu = sum(result_list) / n
        a = 0
        for i in result_list:
            a += i * i
        sigma = math.sqrt(a / n - mu * mu)
        center.append(mu)
        sigma3.append(3 * sigma)
        # print('Gussian Distribution Range : (' + str(mu - 3 * sigma) + ', ' + str(mu + 3 * sigma) + ')')
        
    # print('Center: (' + str(center[0]) + ', ' + str(center[1]) + ', ' + str(center[2]) + ')')
    # print('3sigma: (' + str(sigma3[0]) + ', ' + str(sigma3[1]) + ', ' + str(sigma3[2]) + ')')
    print(feature + ':  (' + str(center[0]) + '+-' + str(sigma3[0]) + ') (' + str(center[1]) + '+-' + str(sigma3[1]) + ') (' + str(center[1]) + '+-' + str(sigma3[1]) + ')')
    outFile = open(feature_path, 'w')
    for i in [0, 1, 2]:
        outFile.write(str(center[i]) + ' ')
        outFile.write(str(sigma3[i]) + '\n')
    outFile.close()

    print(feature + ' finished.')
