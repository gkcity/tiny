#!/bin/sh

export STAGING_DIR="/opt/OpenWrt-SDK-ramips-mt7688_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/staging_dir/"

TARGET=target_openwrt_mt7688
rm -fr $TARGET
mkdir $TARGET
cd $TARGET
cmake ../../ -DCMAKE_TOOLCHAIN_FILE=../openwrt_mt7688.cmake
make
ls -l -h ../../target/
