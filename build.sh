#!/bin/bash

core=`cat /proc/cpuinfo | grep cores | wc -l`

./autogen.sh
cd depends
make HOST=x86_64-unknown-linux-gnu -j$core
cd ..
./configure --prefix=`pwd`/depends/x86_64-unknown-linux-gnu --enable-cxx --enable-shared --enable-static --with-pic
make -j $core


