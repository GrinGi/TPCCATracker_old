rm -rf build
mkdir build
cd build
rm -rf ./*
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_TBB=FALSE ../
make CA CA-draw CA-draw3 -j
