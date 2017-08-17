#!/bin/sh

rm -fr linux_arm
mkdir linux_arm
cd linux_arm
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../linux_arm.cmake
make
ls -l -h ./output
