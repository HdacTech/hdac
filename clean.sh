cd depends
make clean
cd ..
make clean
find . -name *.a -exec rm -rf {} +
find . -name .deps -exec rm -rf {} +
find . -name .dirstamp -exec rm -rf {} +

