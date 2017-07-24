import os

# Define
model_dir = 'E:\\data\\stl'
train_dir = "E:\\trainset"

file_list = os.listdir(model_dir)

for single_file in file_list:
    os.makedirs(train_dir + '\\' + single_file)
    os.system('..\\execute\\feature.exe ' + model_dir + '\\' + single_file + '\\teeth_stl ' + train_dir + '\\' + single_file + '\\ 1')
