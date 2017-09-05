import tensorflow as tf
import reader
import sys
import numpy as np

id = sys.argv[1]
tmpPath = sys.argv[2]

m_id = str(int(id) // 7 + 1) + str(int(id) % 7 + 1)

print(m_id)

sess = tf.Session()    
saver = tf.train.import_meta_graph('E:/cnnModel/' + str(m_id) + '/cnn_model.meta')
saver.restore(sess,tf.train.latest_checkpoint('E:/cnnModel/' + str(m_id) + '/'))

graph = tf.get_default_graph()
xs = graph.get_tensor_by_name("xs_to_restore:0")
keep_prob = graph.get_tensor_by_name("prob_to_restore:0")
prediction = graph.get_tensor_by_name("prediction_to_restore:0")

x = reader.readbmp(tmpPath, [id])
x -= np.mean(x)
x /= np.std(x)
y = sess.run(prediction, feed_dict = {xs: x, keep_prob: 1})

y = [int(64.0 * i) for i in y[0]]

# print(y)

output = open(tmpPath + id + '.txt', 'w')
for i in y:
    output.write(str(i) + ' ')
output.close()
