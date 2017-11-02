#!/bin/sh

TARGET=target_linux_x86
rm -fr $TARGET
mkdir $TARGET
cd $TARGET
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../linux_x86.cmake
make
ls -l -h ../../target/
