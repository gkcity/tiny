MESSAGE(STATUS "make for arm-fsl-linux")
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_SYSTEM_PROCESSOR arm-fsl)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#----------------------------------------------------------------------------
#
# Toolchain Settings
#
#----------------------------------------------------------------------------
SET(TOOLCHAIN_ROOT "/opt/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain")
SET(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_ROOT})
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_C_COMPILER ${TOOLCHAIN_ROOT}/bin/arm-fsl-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLCHAIN_ROOT}/bin/arm-fsl-linux-gnueabi-g++)
SET(CMAKE_RANLIB ${TOOLCHAIN_ROOT}/bin/arm-fsl-linux-ranlib)
SET(CMAKE_AR ${TOOLCHAIN_ROOT}/bin/arm-fsl-linux-ar)
SET(CMAKE_STRIP ${TOOLCHAIN_ROOT}/bin/arm-fsl-linux-strip)

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
ADD_DEFINITIONS(-D__LINUX__)

#----------------------------------------------------------------------------
#
# DEBUG INFORMATION
#
#----------------------------------------------------------------------------
#ADD_DEFINITIONS(-DTINY_DEBUG)

#----------------------------------------------------------------------------
#
# TinyThread
#
#----------------------------------------------------------------------------
SET(TINY_THREAD_ENABLED 1)

#----------------------------------------------------------------------------
#
# Export library
#
#----------------------------------------------------------------------------
SET(EXPORT_STATIC_LIBRARY 0)
SET(EXPORT_SHARED_LIBRARY 1)
