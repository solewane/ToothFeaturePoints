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
print(feature_list)

for feature in feature_list:
    if feature[0:2] in ['11', '12', '13', '15', '16']:
        continue
    
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
