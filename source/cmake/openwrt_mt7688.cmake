MESSAGE(STATUS "make for OpenWrt")
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_SYSTEM_PROCESSOR mt7688)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#----------------------------------------------------------------------------
#
# Toolchain Settings
#
#----------------------------------------------------------------------------
SET(TOOLCHAIN_ROOT "/opt/OpenWrt-Toolchain-ramips-mt7688_gcc-4.8-linaro_uClibc-0.9.33.2.Linux-x86_64/toolchain-mipsel_24kec+dsp_gcc-4.8-linaro_uClibc-0.9.33.2")
SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_ROOT})
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_C_COMPILER ${TOOLCHAIN_ROOT}/bin/mipsel-openwrt-linux-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_ROOT}/bin/mipsel-openwrt-linux-g++)
SET(CMAKE_RANLIB ${TOOLCHAIN_ROOT}/bin/mipsel-openwrt-linux-ranlib)
SET(CMAKE_AR ${TOOLCHAIN_ROOT}/bin/mipsel-openwrt-linux-ar)
SET(CMAKE_STRIP ${TOOLCHAIN_ROOT}/bin/mipsel-openwrt-linux-strip)

#----------------------------------------------------------------------------
#
# for GCC
#
#----------------------------------------------------------------------------
ADD_DEFINITIONS(-Wno-multichar)
ADD_DEFINITIONS(-Wno-pointer-to-int-cast)
ADD_DEFINITIONS(-O1)
ADD_DEFINITIONS(-fPIC)
ADD_DEFINITIONS(-std=c99)
ADD_DEFINITIONS(-D_GNU_SOURCE)
ADD_DEFINITIONS(-D__OPENWRT_MT7688__)

#----------------------------------------------------------------------------
#
# DEBUG INFORMATION
#
#----------------------------------------------------------------------------
ADD_DEFINITIONS(-DTINY_DEBUG)

#----------------------------------------------------------------------------
#
# TinyThread
#
#----------------------------------------------------------------------------
SET(TINY_THREAD_ENABLED 1)
