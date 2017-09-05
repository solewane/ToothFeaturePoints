import math

files = [ 2, 4, 6, 7, 8, 9, 11, 13, 15, 16 ]
origin_folder = 'E:/data/feature_points/'
predict_folder = 'E:/validate/'

output_file = 'E:/result.txt'

xml_file = []
with open('../extract/all.xml', 'r', encoding='UTF-8') as inFile:
    lines = inFile.readlines()
for single_line in lines:
    start_pos = single_line.find('\" name=\"') + 8
    end_pos = single_line.find('\" is')
    if start_pos != -1 and end_pos != -1:
        xml_file.append(single_line[start_pos:end_pos])

outFile = open(output_file, 'w')

for feature in xml_file:
    dis = []
    for single_file in files:
        origin_file = origin_folder + str(single_file) + '.txt'
        predict_file = predict_folder + str(single_file) + '.txt'
        origin = [ 0, 0, 0 ]
        predict = [ 0, 0, 0 ]
        origin_flag = 0
        predict_flag = 0
        with open(origin_file, 'r') as inFile:
            lines = inFile.readlines()
        for single_line in lines:
            if single_line.find(feature) != -1:
                single_line_arr = single_line.split()
                origin[0] = eval(single_line_arr[1].split(',')[0])
                origin[1] = eval(single_line_arr[2].split(',')[0])
                origin[2] = eval(single_line_arr[3].split(',')[0])
                origin_flag = 1
        with open(predict_file, 'r') as inFile:
            lines = inFile.readlines()
        for single_line in lines:
            if single_line.find(feature) != -1:
                single_line_arr = single_line.split()
                predict[0] = eval(single_line_arr[1])
                predict[1] = eval(single_line_arr[2])
                predict[2] = eval(single_line_arr[3])
                predict_flag = 1
        if origin_flag == 1 and predict_flag == 1:
            single_dis = 0.0
            for i in [0, 1, 2]:
                single_dis += (origin[i] - predict[i]) * (origin[i] - predict[i])
            dis.append(math.sqrt(single_dis))
    if len(dis) != 0:
        outFile.write(feature)
        for i in dis:
            outFile.write(' ' + str(i))
        outFile.write('\n')

outFile.close()
