#!/bin/bash

core=`cat /proc/cpuinfo | grep cores | wc -l`

./autogen.sh
cd depends
make HOST=x86_64-w64-mingw32 -j$core
cd ..
./configure --prefix=`pwd`/depends/x86_64-w64-mingw32 --enable-cxx --disable-shared --enable-static --with-pic
make -j $core


