#!/bin/sh

TARGET=target_rtos_esp8266
rm -fr $TARGET
mkdir $TARGET
cd $TARGET
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../freertos_esp8266_xtensa-lx106.cmake
make
ls -l -h ../../target/
