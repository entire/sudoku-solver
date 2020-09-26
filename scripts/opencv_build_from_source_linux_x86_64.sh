#!/usr/bin/env bash

set -ex
OPENCV_VERSION=4.3.0

INSTALL_DIR="$HOME/opt/opencv4"

# first check if opencv_build folder exists
if [ -d $INSTALL_DIR ] 
then
    echo "Directory $INSTALL_DIR already exists." 
else
    echo "Error: Directory $INSTALL_DIR does not exist yet."
    mkdir -p $INSTALL_DIR
fi

# install needed libraries
sudo apt install cmake git pkg-config

# check if opencv exists
if [ -d "$INSTALL_DIR/opencv" ]
then
    echo "opencv dir already eists"
else
    cd $INSTALL_DIR && git clone https://github.com/opencv/opencv.git
    git checkout $OPENCV_VERSION
fi

if [ -d "$INSTALL_DIR/opencv_contrib" ]
then
    echo "opencv_contrib dir already eists"
else
    cd $INSTALL_DIR && git clone https://github.com/opencv/opencv_contrib.git
    git checkout $OPENCV_VERSION
fi

if [ -d "$INSTALL_DIR/opencv/build" ]
then
    echo "build already exists"
    cd $INSTALL_DIR/opencv/build
else 
    mkdir -p $INSTALL_DIR/opencv/build
fi

CONDA_HOME=~/miniconda3 # assume that you're using miniconda
CONDA_ENV="source_cv4_py36"
CONDA_ENV_PATH=$CONDA_HOME/envs/$CONDA_ENV

cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D ENABLE_CXX_11=ON \
    -D ENABLE_PRECOMPILED_HEADERS=OFF \
    -D ENABLE_FAST_MATH=ON \
    -D HAVE_OPENCV_PYTHON3=ON \
    -D INSTALL_C_EXAMPLES=OFF \
    -D INSTALL_PYTHON_EXAMPLES=ON \
    -D OPENCV_EXTRA_MODULES_PATH=$INSTALL_DIR/opencv_contrib/modules/ \
    -D OPENCV_ENABLE_NONFREE=ON \
    -D OPENCV_GENERATE_PKGCONFIG=ON \
    -D BUILD_PROTOBUF=OFF \
    -D BUILD_OPENCV_DNN=OFF \
    -D BUILD_OPENCV_LEGACY=OFF \
    -D BUILD_OPENCV_PYTHON2=OFF \
    -D BUILD_OPENCV_PYTHON3=ON \
    -D BUILD_NEW_PYTHON_SUPPORT=ON \
    -D BUILD_PYTHON_SUPPORT=ON \
    -D BUILD_EXAMPLES=ON \
    -D BUILD_TIFF=ON \
    -D BUILD_opencv_java=OFF \
    -D PYTHON3_EXECUTABLE=$CONDA_ENV_PATH/bin/python3.6 \
    -D PYTHON3_NUMPY_INCLUDE_DIRS=$CONDA_ENV_PATH/lib/python3.6/site-packages \
    -D PYTHON_DEFAULT_AVAILABLE=$CONDA_ENV_PATH/bin/python3.6 \
    -D PYTHON3_INCLUDE_DIR=$CONDA_ENV_PATH/include/python3.6m \
    -D PYTHON3_PACKAGES_PATH=$CONDA_ENV_PATH/lib/python3.6/site-packages \
    -D PYTHON3_LIBRARY=$CONDA_ENV_PATH/lib/libpython3.6m.so \
    -D PYTHON_DEFAULT_EXECUTABLE=$CONDA_ENV_PATH/bin/python3.6 \
    -D PYTHON_PACKAGES_PATH=$CONDA_ENV_PATH/lib/python3.6/site-packages \-D WITH_TBB=ON \
    -D WITH_QT=ON \
    -D WITH_CUDA=OFF \
    -D WITH_OPENGL=ON \
    -D WITH_OPENCL=ON \
    -D WITH_IPP=ON \
    -D WITH_V4L=ON \
    -D WITH_VTK=OFF \
    -D BUILD_TESTS=OFF ..

make -j$(nproc)
sudo make install
sudo ldconfig

    