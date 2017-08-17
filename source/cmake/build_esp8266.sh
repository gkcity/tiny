#!/bin/sh

BUILD_OUTPUT=build_esp8266_debug
rm -fr $BUILD_OUTPUT
mkdir $BUILD_OUTPUT
cd $BUILD_OUTPUT
echo $PWD
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../esp8266_xtensa-lx106.cmake
make
ls -l -h ./output
