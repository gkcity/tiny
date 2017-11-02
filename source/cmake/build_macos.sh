#!/bin/sh

TARGET=target_macos
rm -fr $TARGET
mkdir $TARGET
cd $TARGET
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../macos.cmake
make
ls -l -h ../../target/
