import os
import math

# Define
model_dir = 'E:\\data\\stl'
file_dir = 'E:\\data\\feature_points'
tooth_id = '11'
feature_id = 'center_distal'

feature = tooth_id + '_' + feature_id
output_dir = 'E:\\data\\' + feature
output_dir_absolute = 'E:\\data\\' + feature + '\\absolute'

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
            print('Open file err!')
    minCor = min(result_list)
    maxCor = max(result_list)
    
    for j in range(0, maxCor - minCor + 1):
        result_graph.append(0)
    for j in result_list:
        result_graph[j - minCor] += 1
    
    print(str(i) + ' :')
    print(result_graph)
    print('Min : ' + str(minCor))
    print('Max : ' + str(maxCor))

    # Calcuate Gaussian Distribution
    n = len(result_list)
    mu = sum(result_list) / n
    a = 0
    for i in result_list:
        a += i * i
    sigma = math.sqrt(a / n - mu * mu)

    print('Gussian Distribution Range : (' + str(mu - 3 * sigma) + ', ' + str(mu + 3 * sigma) + ')')
