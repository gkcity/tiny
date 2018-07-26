#!/bin/sh

TARGET=target_linux_fsl
rm -fr $TARGET
mkdir $TARGET
cd $TARGET
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../linux_arm-fsl.cmake
make --trace
ls -l -h ../../target/
