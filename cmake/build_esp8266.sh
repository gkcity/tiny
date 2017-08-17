#!/bin/sh

rm -fr build_esp8266
mkdir build_esp8266
cd build_esp8266
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../esp8266_xtensa-lx106.cmake
make
ls -l -h output/
