export LDFLAGS=-L/usr/local/opt/openssl/lib
export CPPFLAGS=-I/usr/local/opt/openssl/include

./autogen.sh
cd depends
make HOST=x86_64-apple-darwin17.3.0 -j4
cd ..
./configure --prefix=`pwd`/x86_64-apple-darwin17.3.0 --with-gui=no --with-libs=no --with-miniupnpc=no --enable-cxx --enable-shared --enable-static --with-pic
make -j 4


