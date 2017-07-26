import os

# Define
model_dir = 'E:\\data\\stl'
feature_dir = 'E:\\data\\feature_points'
output_dir = 'E:\\trainset'

file_list = os.listdir(feature_dir)
feature_list = []

xml_file = []
with open('..\\extract\\all.xml', 'r', encoding='UTF-8') as inFile:
    lines = inFile.readlines()
for single_line in lines:
    start_pos = single_line.find('\" name=\"') + 8
    end_pos = single_line.find('\" is')
    if start_pos != -1 and end_pos != -1:
        xml_file.append(single_line[start_pos:end_pos])

for file_name in file_list:
    with open(feature_dir + '\\' + file_name, 'r') as inFile:
        file_lines = inFile.readlines()
    for single_line in file_lines:
        end_pos = single_line.find(':')
        tmp = single_line[0:end_pos]
        if (tmp not in feature_list) and (tmp in xml_file):
            feature_list.append(tmp)

for feature in feature_list:
    out_file = open(output_dir + '\\' + feature + '.txt', 'w')
    out_file.close()

file_list = os.listdir(model_dir)
for file_name in file_list:
    os.system('..\\execute\\feature.exe ' + model_dir + '\\' + file_name + '\\teeth_stl ' + output_dir + ' 1')
    print(file_name + ' Finished.')
