# LLLMarker


Compile QR Detector + openFrameworks

# LLLMarker libraries dependencies installation

OS:

## Mac Installation

brew
````
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install cmake
brew install freetype
brew install harfbuzz
brew install hdf5
brew install libpng
brew install libsmf
brew install ceres-solver
brew install eigen glog gflags
brew install vtk, viz
brew install pkg-config
brew install tbb

pip install --upgrade pip setuptools
````

Permission to pkgconfig to brew link formulas
````
sudo chown -R $(whoami) /usr/local/lib/pkgconfig
````

openframeworks

https://github.com/openframeworks/openFrameworks

https://github.com/openframeworks/openFrameworks/tree/c274c7fb51b4ae0552cd4cdb00475458aeeb610a


Download opencv and install

````
git clone https://github.com/opencv/opencv
git clone https://github.com/opencv/opencv_contrib
cd opencv
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules  ..
make -j8
sudo make install
````

#### ERORR

````
dyld: Library not loaded: /usr/local/lib/libjpeg.8.dylib
  Referenced from: /usr/local/opt/opencv/lib/libopencv_highgui.2.4.dylib
  Reason: image not found
/bin/sh: line 1: 23246 Abort trap: 6           ./ArucoTracker
````

brew switch libjpeg 8d

https://stackoverflow.com/questions/32703296/dyld-library-not-loaded-usr-local-lib-libjpeg-8-dylib-homebrew-php

````
wget -c http://www.ijg.org/files/jpegsrc.v8d.tar.gz
tar xzf jpegsrc.v8d.tar.gz
cd jpeg-8d
./configure
make
cp ./.libs/libjpeg.8.dylib /usr/local/opt/jpeg/lib
````

## Linux install

Step 1: Update packages

````
sudo apt-get update
sudo apt-get upgrade
````

We will Install dependencies now

````
sudo apt-get install build-essential checkinstall cmake pkg-config yasm
sudo apt-get install git gfortran
sudo apt-get install libjpeg8-dev libjasper-dev libpng12-dev
 
sudo apt-get install libtiff5-dev
 
sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev libdc1394-22-dev
sudo apt-get install libxine2-dev libv4l-dev
sudo apt-get install libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev
sudo apt-get install qt5-default libgtk2.0-dev libtbb-dev
sudo apt-get install libatlas-base-dev
sudo apt-get install libfaac-dev libmp3lame-dev libtheora-dev
sudo apt-get install libvorbis-dev libxvidcore-dev
sudo apt-get install libopencore-amrnb-dev libopencore-amrwb-dev
sudo apt-get install x264 v4l-utils

sudo apt-get install libprotobuf-dev protobuf-compiler
sudo apt-get install libgoogle-glog-dev libgflags-dev
sudo apt-get install libgphoto2-dev libeigen3-dev libhdf5-dev doxygen
````

Step 3: Install Python libraries

````
sudo apt-get install python-dev python-pip python3-dev python3-pip
sudo -H pip2 install -U pip numpy
sudo -H pip3 install -U pip numpy
````

### Download and install opencv

````
git clone https://github.com/opencv/opencv
git clone https://github.com/opencv/opencv_contrib
cd opencv
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules  ..
make -j8
sudo make install
````

## Windows installation

openCV

https://www.learnopencv.com/install-opencv3-on-windows/


git clone https://github.com/opencv/opencv
git clone https://github.com/opencv/opencv_contrib

- install cmake
 https://cmake.org/download/

- 

Configure contrib library:
https://stackoverflow.com/questions/37517983/opencv-install-opencv-contrib-on-windows
