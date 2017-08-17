#!/bin/sh

rm -fr build_macos
mkdir build_macos
cd build_macos
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../macos.cmake
make
ls -l -h ./output
