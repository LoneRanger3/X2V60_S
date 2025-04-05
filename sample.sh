#!/bin/bash

RootDirectory=`pwd`
rm -f $RootDirectory/pack/usr/bin/car_demo

# 编译sample
cd $RootDirectory/sample/
if [ -d "build" ]; then
    cd build
    rm -rf *
else
    mkdir build;cd build
fi
#USE_TS=OFF调用libXM_MiddleWare_api_mp4.a，用mp4。USE_TS=ON调用libXM_MiddleWare_api.a，用ts流
cmake .. -DARCH=board650v200 -DCMAKE_BUILD_TYPE=Release -DUSE_TS=ON -DCT317=ON
make clean
make

# 拷贝car_demo并编译固件
arm-xm-linux-strip car_demo
cp car_demo $RootDirectory/pack/usr/bin/
cd $RootDirectory/pack/
make clean
make

cd $RootDirectory/
