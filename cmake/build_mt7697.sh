#!/bin/sh

TARGET=target_rtos_mt7697
rm -fr $TARGET
mkdir $TARGET
cd $TARGET
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../freertos_mt7697.cmake
make
ls -l -h ../../target/
