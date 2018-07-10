# LLLMarker


Compile QR Detector + openFrameworks


https://gist.github.com/leonmak/67a70ccc83e149058c29b51b1ca0d161

## Mac Installation

brew
````
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
brew install cmake
brew install freetype
brew install libpng
````

Permission to pkgconfig to brew link formulas
````
sudo chown -R $(whoami) /usr/local/lib/pkgconfig
````

openframeworks

https://github.com/openframeworks/openFrameworks

https://github.com/openframeworks/openFrameworks/tree/c274c7fb51b4ae0552cd4cdb00475458aeeb610a



Download

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
