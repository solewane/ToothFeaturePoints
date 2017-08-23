import os
import numpy as np
from PIL import Image

class Dataset(object):
    image_path = 'E:\\data\\img\\16\\'
    filenames = []

    xs = []
    ys = []

    total_size = 0
    train_size = 0
    validate_size = 0

    def __init__(self, r):
        print('Read images from ' + self.image_path)
        self.filenames = list(set([i[:i.find('.')] for i in os.listdir(self.image_path)]))
        self.filenames.sort()
        self.total_size = len(self.filenames)
        self.train_size = int(float(self.total_size) * r)
        self.validate_size = self.total_size - self.train_size - 1

    def getData(self):
        self.xs = readbmp(self.image_path, self.filenames)
        self.ys = readPoint(self.image_path, self.filenames)

        self.xs -= np.mean(self.xs, axis = 0)
        self.xs /= np.std(self.xs, axis = 0)

        print('Read complete.')
        self.showInfo()

    def showData(self, i):
        print('Image ' + str(i))
        print(self.xs[i][20])
        print('Feature ' + str(i))
        print(self.ys[i])

    def showInfo(self):
        print('Total size: ' + str(self.total_size))
        print('Train size: ' + str(self.train_size))
        print('Validate size: ' + str(self.validate_size))

    def getTrainData(self):
        return (self.xs[:self.train_size], self.ys[:self.train_size])
    
    def getValidateData(self):
        batch_xs = self.xs[self.train_size:len(self.xs) - 2]
        batch_ys = self.ys[self.train_size:len(self.ys) - 2]
        return (batch_xs, batch_ys)
    
    def getTestData(self, i):
        return (self.xs[i], self.ys[i])


def readbmp(path, filenames):
    imgs = []

    for filename in filenames:
        img = Image.open(path + filename + '.bmp')
        single_img = []
        for i in range(0, 64):
            line = []
            for j in range(0, 64):
                line.append(float(img.getpixel((j, i))[0]))
            single_img.append(line)
        imgs.append(single_img)

    return imgs


def readPoint(path, filenames):
    points = []

    for filename in filenames:
        with open(path + filename + '.txt', 'r') as feature_file:
            lines = feature_file.readlines()
            point1 = [float(eval(i)) / 64.0 for i in lines[0].split()]
            point2 = [float(eval(i)) / 64.0 for i in lines[1].split()]
            points.append(point1 + point2)

    return points
