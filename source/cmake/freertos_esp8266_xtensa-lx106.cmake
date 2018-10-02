MESSAGE(STATUS "make for esp8266")
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_SYSTEM_PROCESSOR xtensa)
SET(CMAKE_FIND_ROOT_PATH "/opt/xtensa-lx106-elf")
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#----------------------------------------------------------------------------
#
# Toolchain Settings
#
#----------------------------------------------------------------------------
SET(CMAKE_C_COMPILER /opt/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc)
SET(CMAKE_CXX_COMPILER /opt/xtensa-lx106-elf/bin/xtensa-lx106-elf-g++)
SET(CMAKE_RANLIB /opt/xtensa-lx106-elf/bin/xtensa-lx106-elf-ranlib)
SET(CMAKE_AR /opt/xtensa-lx106-elf/bin/xtensa-lx106-elf-ar)
SET(CMAKE_C_FLAGS "-nostdlib -Wall -Werror -Os" CACHE STRING "" FORCE)

ADD_DEFINITIONS(-DLWIP_SOCKET)
ADD_DEFINITIONS(-DLWIP_TIMEVAL_PRIVATE=1)
ADD_DEFINITIONS(-DESP8266)
ADD_DEFINITIONS(-DICACHE_FLASH)
INCLUDE_DIRECTORIES("/opt/ESP8266_RTOS_SDK/include")
INCLUDE_DIRECTORIES("/opt/ESP8266_RTOS_SDK/include/lwip")
INCLUDE_DIRECTORIES("/opt/ESP8266_RTOS_SDK/include/lwip/ipv4")
INCLUDE_DIRECTORIES("/opt/ESP8266_RTOS_SDK/include/lwip/ipv6")
INCLUDE_DIRECTORIES("/opt/ESP8266_RTOS_SDK/include/espressif")

#----------------------------------------------------------------------------
#
# for GCC
#
#----------------------------------------------------------------------------
ADD_DEFINITIONS(-Wno-multichar)
ADD_DEFINITIONS(-Wno-pointer-to-int-cast)
ADD_DEFINITIONS(-O3)
ADD_DEFINITIONS(-fPIC)
ADD_DEFINITIONS(-std=c99)

#----------------------------------------------------------------------------
#
# DEBUG INFORMATION
#
#----------------------------------------------------------------------------
ADD_DEFINITIONS(-DTINY_DEBUG)
ADD_DEFINITIONS(-DTINY_STATIC)