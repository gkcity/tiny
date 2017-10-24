#!/bin/sh

TARGET=target_rtos_esp32
rm -fr $TARGET
mkdir $TARGET
cd $TARGET
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../freertos_esp32_xtensa-esp32-elf.cmake
make
ls -l -h ../../target/
