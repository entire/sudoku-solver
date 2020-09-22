
from tensorflow.keras import backend as K
# this must be exec'd before loading keras model
K.set_learning_phase(0)

# this has to be tensorflow.keras
# or else you'll get this -> https://stackoverflow.com/questions/58878421/unexpected-keyword-argument-ragged-in-keras
from tensorflow.keras.models import load_model
model = load_model('../output/dc.h5')
# quick check
print(model.outputs)
print(model.inputs)

import tensorflow as tf

def freeze_session(session, keep_var_names=None, output_names=None, clear_devices=True):
    """
    Freezes the state of a session into a pruned comuptaion graph
    """
    from tensorflow.python.framework.graph_util import convert_variables_to_constants
    graph = session.graph
    with graph.as_default():
        freeze_var_names = list(set(v.op.name for v in tf.global_variables()).difference(keep_var_names or []))
        output_names = output_names or []
        output_names += [v.op.name for v in tf.global_variables()]
        # graph -> graphdef in protobuf
        input_graph_def = graph.as_graph_def()
        if clear_devices:
            for node in input_graph_def.node:
                node.device = ""
        frozen_graph = convert_variables_to_constants(session, input_graph_def, output_names, freeze_var_names)
        return frozen_graph

if __name__ == "__main__":
    frozen_graph=freeze_session(K.get_session(), output_names=[out.op.name for out in model.outputs])
    tf.train.write_graph(frozen_graph, "model", "tf_model.pb", as_text=False)