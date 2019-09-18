#!/bin/sh

mkdir ./linux
make clean
CROSS_COMPILE=aarch64-linux-gnu- make -j4
mv am3xtest ./linux/am3xtest-arm64

make clean
CROSS_COMPILE=arm-linux-gnueabihf- make -j4
mv am3xtest ./linux/am3xtest-arm32


make clean
CROSS_COMPILE=mips64el-linux-gnuabi64- make -j4
mv am3xtest ./linux/am3xtest-mips64

make clean
CROSS_COMPILE=x86_64-linux-gnu- make -j4
mv am3xtest ./linux/am3xtest-x64

make clean
cp am3xtest.ini ./linux
cp release.txt ./linux/readme.txt

echo "done"
