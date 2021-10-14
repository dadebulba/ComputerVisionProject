#!/bin/bash

to_build=0
to_install_dependencies=0
BUILD_DIR="build"

function usage {
    echo "USAGE: start.sh [options]"
    echo "    --help:    get parameters information"
    echo "    --install: install the dependencies"
    echo "    --build:   build the application"
    echo ""
}

function install {
    # Install dependencies for build
    # https://linuxize.com/post/how-to-install-opencv-on-ubuntu-20-04/

    actual_dir=$(pwd)

    # Update repositories
    echo "--- Updating local repositories ---"
    apt update -y >/dev/null 2>&1
    apt upgrade -y >/dev/null 2>&1

    echo "--- Installing dependecies for build ---"
    apt install build-essential g++ cmake git pkg-config libgtk-3-dev \
    libavcodec-dev libavformat-dev libswscale-dev libv4l-dev \
    libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev \
    gfortran openexr libatlas-base-dev python3-dev python3-numpy \
    libtbb2 libtbb-dev libdc1394-22-dev libopenexr-dev \
    libgstreamer-plugins-base1.0-dev libgstreamer1.0-dev libboost-all-dev libgtk2.0-dev libcanberra-gtk-module -y >/dev/null 2>&1

    # Download the OpenCV library for C++ version 2.4 as required
    echo "--- Downloading OpenCV 2.4 C++ library ---"
    mkdir ~/opencv_build && cd ~/opencv_build
    git clone https://github.com/opencv/opencv.git >/dev/null 2>&1
    cd opencv
    git checkout 2.4 >/dev/null 2>&1
    cd ~/opencv_build/opencv
    mkdir -p build && cd build

    echo "--- Building OpenCV 2.4 ---"
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D INSTALL_C_EXAMPLES=ON \
    -D INSTALL_PYTHON_EXAMPLES=ON \
    -D OPENCV_GENERATE_PKGCONFIG=ON \
    -D OPENCV_EXTRA_MODULES_PATH=~/opencv_build/opencv_contrib/modules \
    -D BUILD_EXAMPLES=ON .. >/dev/null 2>&1

    echo "--- Compiling OpenCV 2.4 (can take several minutes) ---"
    make -j$(nproc) >/dev/null 2>&1

    echo "--- Installing OpenCV 2.4 ---"
    make install >/dev/null 2>&1

    echo "--- OpenCV installation completed - Version $(pkg-config --modversion opencv) ---"
    rm -rf ~/opencv_build
    cd $actual_dir
}

function build {
    echo "--- Building the project ---"
    mkdir $BUILD_DIR

    # Compile each source file
    g++ -I/usr/include/ -I/usr/local/include/opencv/ -O2 -g3 -Wall -w -c -fmessage-length=0 -MMD -MP -MF"main.d" -MT"./$BUILD_DIR/main.o" -o "./$BUILD_DIR/main.o" "./main.cpp"
    g++ -I/usr/include/ -I/usr/local/include/opencv/ -O2 -g3 -Wall -w -c -fmessage-length=0 -MMD -MP -MF"module2.d" -MT"./$BUILD_DIR/module2.o" -o "./$BUILD_DIR/module2.o" "./modules/module2.cpp"
    g++ -I/usr/include/ -I/usr/local/include/opencv/ -O2 -g3 -Wall -w -c -fmessage-length=0 -MMD -MP -MF"module3.d" -MT"./$BUILD_DIR/module3.o" -o "./$BUILD_DIR/module3.o" "./modules/module3.cpp"
    g++ -I/usr/include/ -I/usr/local/include/opencv/ -O2 -g3 -Wall -w -c -fmessage-length=0 -MMD -MP -MF"mmGaussianModel.d" -MT"./$BUILD_DIR/mmGaussianModel.o" -o "./$BUILD_DIR/mmGaussianModel.o" "./mmAnomaly/mmGaussianModel.cpp"
    g++ -I/usr/include/ -I/usr/local/include/opencv/ -O2 -g3 -Wall -w -c -fmessage-length=0 -MMD -MP -MF"mmParticleAccumulation.d" -MT"./$BUILD_DIR/mmParticleAccumulation.o" -o "./$BUILD_DIR/mmParticleAccumulation.o" "./mmAnomaly/mmParticleAccumulation.cpp"
    g++ -I/usr/include/ -I/usr/local/include/opencv/ -O2 -g3 -Wall -w -c -fmessage-length=0 -MMD -MP -MF"mmParticleEnergy.d" -MT"./$BUILD_DIR/mmParticleEnergy.o" -o "./$BUILD_DIR/mmParticleEnergy.o" "./mmAnomaly/mmParticleEnergy.cpp"

    # Linker
    g++ -L/usr/local/lib/ -o "mmAnomalyExecutable" ./$BUILD_DIR/module2.o ./$BUILD_DIR/module3.o  ./$BUILD_DIR/mmGaussianModel.o ./$BUILD_DIR/mmParticleAccumulation.o ./$BUILD_DIR/mmParticleEnergy.o  ./$BUILD_DIR/main.o -lopencv_calib3d -lopencv_ml -lopencv_highgui -lopencv_legacy -lopencv_video -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_imgproc

    # Clean phase
    rm -rf $BUILD_DIR
    rm ./main.d ./module2.d ./module3.d ./mmGaussianModel.d ./mmParticleAccumulation.d ./mmParticleEnergy.d
    /sbin/ldconfig -v >/dev/null 2>&1

    echo "--- Build completed ---"
}

# Check if the script is running with root privileges
if [[ $(whoami) != "root" ]]; then
    echo "ERROR: run start.sh with root privileges"
    exit 1
fi

# Parse the inline parameters
while [[ $# -gt 0 ]]; do
    case $1 in
        --install )
            to_install_dependencies=1
            to_build=1
        ;;
        --build )
            to_build=1
        ;;              
        --help )    
            usage
            exit
        ;;
        * )              
            usage
            exit 1
        ;;
    esac
    shift
done

# If we need to install the dependencies
if [[ $to_install_dependencies -eq 1 ]]; then
    install
fi

# If we need to build the application
if [[ $to_build -eq 1 ]]; then
    build
    exit
fi

# If both not defined, print usage
usage