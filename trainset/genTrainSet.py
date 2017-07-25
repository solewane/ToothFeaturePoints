import os

# Define
feature_dir = 'E:\\data\\feature_points'

file_list = os.listdir(feature_dir)
feature_list = []

for file_name in file_list:
    with open(feature_dir + '\\' + file_name, 'r') as inFile:
        file_lines = inFile.readlines()
    for single_line in file_lines:
        end_pos = single_line.find(':')
        tmp = single_line[0:end_pos]
        if tmp not in feature_list:
            feature_list.append(tmp)
print(feature_list)