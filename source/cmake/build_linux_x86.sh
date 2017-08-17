#!/bin/sh

rm -fr build_linux_x86
mkdir build_linux_x86
cd build_linux_x86
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../linux_x86.cmake
make
ls -l -h ./output
