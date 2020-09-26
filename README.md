# sudoku_solver


## Requirements for `digit_classifier`
- Tensorflow 2.3.0 
- Keras 2.3.1
- scikit-learn 0.23.1
- Jupyter Notebook

## Requirements for `digit_recognizer`
- OpenCV 4.1.0 (make sure you don't include Protobuf with OpenCV `)
- [Tensorflow C Library 2.3.0 for Linux CPU](https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-cpu-linux-x86_64-2.3.0.tar.gz)
- [cppflow (runs tensorflow models in C++ without bazel)](https://github.com/serizba/cppflow)
- Protobuf (at least 3.9.0)
- Jupyter Notebook
- GoogleTest

## Requirements for `solver`
- CMake 3.14.0

# Major components that it's split into

### digit_classifier
- used MNIST dataset for digit classification
- initially tried keras + tensorflow, ran into issues with Tensorflow 2.3.0, and Keras. The Tensorflow C++ integration wasn't as smooth as I hoped
- ended up writing the model and training etc, that worked just fine.
- instead used pytorch, which worked well

### digit_recognizer
- opencv 

### sudoku solver
