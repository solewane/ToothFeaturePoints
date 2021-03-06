import reader
import tensorflow as tf
import random
import sys
import numpy as np

id = sys.argv[1]

d = reader.Dataset(0.95, id)
d.getData()


# Start training

def compute_accuracy(v_xs, v_ys):
    global prediction
    y_pre = sess.run(prediction, feed_dict = {xs: v_xs, keep_prob: 1})
    # accuracy = tf.reduce_mean(tf.square(tf.subtract(y_pre, v_ys)))
    accuracy = tf.losses.mean_squared_error(y_pre, v_ys)
    result = sess.run(accuracy, feed_dict = {xs: v_xs, ys: v_ys, keep_prob: 1})
    return result

def weight_variable(shape, w):
    initial = tf.truncated_normal(shape, stddev = w)
    return tf.Variable(initial)

def bias_variable(shape):
    initial = tf.constant(0.1, shape = shape)
    return tf.Variable(initial)

def conv2d(x, W):
    return tf.nn.conv2d(x, W, strides=[1, 1, 1, 1], padding='SAME')

def max_pool_2x2(x):
    return tf.nn.max_pool(x, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')

def avg_pool_2x2(x):
    return tf.nn.avg_pool(x, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')

xs = tf.placeholder(tf.float32, [None, 64, 64], name = 'xs_to_restore')
ys = tf.placeholder(tf.float32, [None, 4])
keep_prob = tf.placeholder(tf.float32, name = 'prob_to_restore')
x_image = tf.reshape(xs, [-1, 64, 64, 1])

## conv1 layer ##
W_conv1 = weight_variable([5, 5, 1, 32], 0.0001) # patch 5x5, in size 1, out size 32
b_conv1 = bias_variable([32])
h_conv1 = tf.nn.relu(conv2d(x_image, W_conv1) + b_conv1) # output size 64x64x32
h_pool1 = max_pool_2x2(h_conv1) # output size 32x32x32

## conv2 layer ##
W_conv2 = weight_variable([5, 5, 32, 32], 0.01) # patch 5x5, in size 32, out size 32
b_conv2 = bias_variable([32])
h_conv2 = tf.nn.relu(conv2d(h_pool1, W_conv2) + b_conv2) # output size 32x32x32
h_pool2 = avg_pool_2x2(h_conv2) # output size 16x16x32

## conv3 layer ##
W_conv3 = weight_variable([5, 5, 32, 64], 0.01) # patch 5x5, in size 32, out size 64
b_conv3 = bias_variable([64])
h_conv3 = tf.nn.relu(conv2d(h_pool2, W_conv3) + b_conv3) # output size 16x16x64
h_pool3 = avg_pool_2x2(h_conv3) # output size 8x8x64

## func1 layer ##
W_fc1 = weight_variable([8 * 8 * 64, 4096], 0.1)
b_fc1 = bias_variable([4096])
# [n_samples, 8, 8, 64] ->> [n_samples, 8*8*64]
h_pool3_flat = tf.reshape(h_pool3, [-1, 8 * 8 * 64])
h_fc1 = tf.nn.relu(tf.matmul(h_pool3_flat, W_fc1) + b_fc1)
h_fc1_drop = tf.nn.dropout(h_fc1, keep_prob)

## func2 layer ##
W_fc2 = weight_variable([4096, 4], 0.1)
b_fc2 = bias_variable([4])
# prediction = tf.nn.softmax(tf.matmul(h_fc1_drop, W_fc2) + b_fc2)
prediction = tf.reshape(tf.matmul(h_fc1_drop, W_fc2) + b_fc2, [-1, 4], name = 'prediction_to_restore')

## arguments ##
loss = tf.losses.mean_squared_error(prediction, ys)
# loss = tf.reduce_mean(tf.square(tf.subtract(prediction, ys)))
train_step = tf.train.GradientDescentOptimizer(1e-4).minimize(loss)

sess = tf.Session()

sess.run(tf.global_variables_initializer())

for i in range(1000):
    batch = d.getTrainData()
    batch_xs = batch[0]
    batch_ys = batch[1]
    sess.run(train_step, feed_dict = {xs: batch_xs, ys: batch_ys, keep_prob: 0.5})
    if i % 100 == 0:
        test_batch = d.getValidateData()
        print(str(i) + ': ' + str(compute_accuracy(test_batch[0], test_batch[1])))

'''
test_data = d.getTestData(len(d.xs) - 1)
y_pre = sess.run(prediction, feed_dict = {xs: [test_data[0]], keep_prob: 1})
print(y_pre[0])
print(test_data[1])
'''

saver = tf.train.Saver()
saver.save(sess, 'E:/cnnModel/' + str(id) + '/cnn_model')
