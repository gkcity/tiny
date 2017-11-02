#!/bin/sh

TARGET=target_linux_arm
rm -fr $TARGET
mkdir $TARGET
cd $TARGET
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../linux_arm.cmake
make
ls -l -h ../../target/
