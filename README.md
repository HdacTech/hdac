Hdac Core
==========

Hdac is licensed under the GPLv3 open source license. 
We also offer commercial licenses for those wishing to integrate Hdac 
into a proprietary software solution.

    Copyright (c) 2017 Hdac Technology AG
    License: GNU General Public License version 3, see COPYING

    Portions copyright (c) 2009-2016 The Bitcoin Core developers
    Portions copyright many others - see individual files

    Portions copyright (c) 2014-2017 Coin Sciences Ltd
    Portions copyright many others - see individual files


System requirements
-------------------

    C++ compilers are memory-hungry, 
    so it is recommended to have at least 1 GB of memory available when compiling Hdac.


Linux BUILD ( on Ubuntu 16.04 x64 )
-----------

  1. Install dependencies
    
    sudo apt-get update
    sudo apt-get install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils
    sudo apt-get install libboost-all-dev
    sudo apt-get install git
    sudo apt-get install software-properties-common
    sudo add-apt-repository ppa:bitcoin/bitcoin
    sudo apt-get update
    sudo apt-get install libdb4.8-dev libdb4.8++-dev


  2. Compile Hdac for Ubuntu (64-bit)
    
    ./build.sh
    
  3. If you are having trouble compiling, please try below

    ./clean.sh
    ./autogen.sh
    ./configure
    make
   
  4. run 

   This will build `hdacd`, `hdac-cli` and `hdac-util` in the `src` directory.

   The release is built with GCC after which `strip hdacd` strings the debug symbols, 
   which reduces the executable size by about 90%.

  5. Clean build

    ./clean.sh
    


BUILD for Windows 7 or 10 ( on Ubuntu 16.04 )
---------------------------------------------


  1. Install dependencies

    sudo apt-get update
    sudo apt-get install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils
    sudo apt-get install g++-mingw-w64-i686 mingw-w64-i686-dev g++-mingw-w64-x86-64 mingw-w64-x86-64-dev curl
    sudo apt-get install libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev libboost-thread-dev
    sudo apt-get install git
    sudo add-apt-repository ppa:bitcoin/bitcoin
    sudo apt-get update
    sudo apt-get install libdb4.8-dev libdb4.8++-dev

  2. Compile Hdac for Windows (64-bit)

    ./win_build.sh    
    
  3. If you are having trouble compiling, please try below

    ./clean.sh
    ./autogen.sh
    cd depends
    make HOST=x86_64-w64-mingw32 -j4
    cd ..
    ./configure --prefix=`pwd`/depends/x86_64-w64-mingw32 --enable-cxx --disable-shared --enable-static --with-pic
    make

  4. run

   This will build `hdacd.exe`, `hdac-cli.exe` and `hdac-util.exe` in the `src` directory.



Linux BUILD ( on CentOS 7 x86_64 )
----------------------------------

   1. Install dependencies

     yum groupinstall "Development Tools"
     yum install wget
     
     
   2. Compile Hdac for CentOS 7 (64-bit)

    ./build.sh
     
   3. run

   This will build `hdacd`, `hdac-cli` and `hdac-util` in the `src` directory.

   4. Clean build

    ./clean.sh
     
     
BUILD for Windows 7 or 10 (on CentOS 7 )
----------------------------------------


   1. Install dependencies

     yum groupinstall "Development Tools"
     yum install wget
     yum install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
     yum install mingw64-*
 
 
   2. Compile Hdac for Windows

    ./win_build.sh     

   3. run

   This will build `hdacd.exe`, `hdac-cli.exe` and `hdac-util.exe` in the `src` directory.


Mac Build Notes (on MacOS Sierra)
---------------------------------

   1. Install dependencies

    Install XCode and XCode command line tools
    Install git from git-scm
    Install brew (follow instructions on brew.sh)
    brew install autoconf automake berkeley-db4 libtool boost openssl pkg-config rename

   2. Compile Hdac for Mac (64-bit)

    ./mac_build.sh
    

   3. If you are having trouble compiling, please try below

    export LDFLAGS=-L/usr/local/opt/openssl/lib
    export CPPFLAGS=-I/usr/local/opt/openssl/include
    ./autogen.sh
    ./configure --with-gui=no --with-libs=no --with-miniupnpc=no
    make

   4. run

   This will build `hdacd`, `hdac-cli` and `hdac-util` in the `src` directory.


Running Core Daemon
-------------------
[/doc/RUN.md](/doc/RUN.md) contains relevant infomation on How to run Hdac on your native platform. ( Linux, windows, Mac OS )

