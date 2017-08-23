import tensorflow as tf
import reader

sess = tf.Session()    
saver = tf.train.import_meta_graph('cnn_model.meta')
saver.restore(sess,tf.train.latest_checkpoint('./'))

graph = tf.get_default_graph()
xs = graph.get_tensor_by_name("xs_to_restore:0")
keep_prob = graph.get_tensor_by_name("prob_to_restore:0")
prediction = graph.get_tensor_by_name("prediction_to_restore:0")

x = reader.readbmp('./', ['input'])
y = sess.run(prediction, feed_dict = {xs: x, keep_prob: 1})

y = [int(64.0 * i) for i in y[0]]

# print(y)

output = open('output.txt', 'w')
for i in y:
    output.write(str(i) + ' ')
output.close()
