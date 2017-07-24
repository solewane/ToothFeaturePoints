import os
import math

# Define
model_dir = 'E:\\data\\stl'
file_dir = 'E:\\data\\feature_points'
feature_dir = 'E:\\data\\pos_feature'

file_list = os.listdir(file_dir)
feature_list = []

for file_name in file_list:
    with open(file_dir + '\\' + file_name, 'r') as inFile:
        file_lines = inFile.readlines()
    for single_line in file_lines:
        end_pos = single_line.find(':')
        tmp = single_line[0:end_pos]
        if tmp not in feature_list:
            feature_list.append(tmp)

pos = 0
for i in range(0, len(feature_list)):
    if feature_list[i] == '46_mesial':
        pos = i
feature_list = feature_list[(pos + 1):]

for feature in feature_list:
    output_dir = 'E:\\data\\' + feature
    output_dir_absolute = 'E:\\data\\' + feature + '\\absolute'
    feature_path = feature_dir + '\\' + feature + '.txt'
    os.makedirs(output_dir_absolute)

    tooth_id = feature[0:feature.find('_')]

    # Extract certain feature from file
    file_list = os.listdir(file_dir)

    for file_name in file_list:
        outFile = open(output_dir_absolute + '\\' + file_name, 'w')
        with open(file_dir + '\\' + file_name, 'r') as inFile:
            file_lines = inFile.readlines()
        for single_line in file_lines:
            if single_line.find(feature) == 0:
                begin_pos = single_line.find(': ')
                outFile.write(single_line[begin_pos + 2:].replace(', ', ' '))
        outFile.close()

    # Calculate new Coordination via C++ program
    for file_name in file_list:
        in_filename = output_dir_absolute + '\\' + file_name
        out_filename = output_dir + '\\' + file_name
        end_pos = file_name.find('.txt')
        os.system('..\\execute\\feature.exe ' + model_dir + '\\' + file_name[:end_pos] + '\\teeth_stl ' + output_dir + '\\' + file_name + ' 2 ' + tooth_id + ' ' + in_filename)

    # Get Distribution Graph
    err_small = 0
    err_big = 0

    point_cloud = []
    center = []
    sigma3 = []
    for i in [0, 1, 2]:
        result_graph = []
        result_list = []
        for file_name in file_list:
            try:
                with open(output_dir + '\\' + file_name, 'r') as inFile:
                    file_lines = inFile.readlines()
                for single_line in file_lines:
                    result_list.append(int(list(map(eval, single_line.rstrip('\n').split(' ')))[i]))
            except:
                print('Open file err! ' + output_dir + '\\' + file_name)
        minCor = min(result_list)
        maxCor = max(result_list)
        
        for j in range(0, maxCor - minCor + 1):
            result_graph.append(0)
        for j in result_list:
            result_graph[j - minCor] += 1
        
        '''
        print(str(i) + ' :')
        print(result_graph)
        print('Min : ' + str(minCor))
        print('Max : ' + str(maxCor))
        '''
        point_cloud.append(result_list)

        # Calcuate Gaussian Distribution
        n = len(result_list)
        mu = sum(result_list) / n
        a = 0
        for i in result_list:
            a += i * i
        sigma = math.sqrt(a / n - mu * mu)
        sigma3.append(3 * sigma)

        # print('Gussian Distribution Range : (' + str(mu - 3 * sigma) + ', ' + str(mu + 3 * sigma) + ')')


    n = len(point_cloud[0])
    for i in [0, 1, 2]:
        center.append(sum(point_cloud[i]) / n)
    # print('Center: (' + str(center[0]) + ', ' + str(center[1]) + ', ' + str(center[2]) + ')')
    # print('3sigma: (' + str(sigma3[0]) + ', ' + str(sigma3[1]) + ', ' + str(sigma3[2]) + ')')

    outFile = open(feature_path, 'w')
    for i in [0, 1, 2]:
        outFile.write(str(center[i]) + ' ')
        outFile.write(str(sigma3[i]) + '\n')
    outFile.close()

    print(feature + ' finished.')
