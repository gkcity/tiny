MESSAGE(STATUS "make for esp32")
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_SYSTEM_PROCESSOR xtensa)
SET(CMAKE_FIND_ROOT_PATH "/opt/xtensa-esp32-elf")
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#----------------------------------------------------------------------------
#
# Toolchain Settings
#
#----------------------------------------------------------------------------
SET(CMAKE_C_COMPILER /opt/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc)
SET(CMAKE_CXX_COMPILER /opt/xtensa-esp32-elf/bin/xtensa-esp32-elf-g++)
SET(CMAKE_RANLIB /opt/xtensa-esp32-elf/bin/xtensa-esp32-elf-ranlib)
SET(CMAKE_AR /opt/xtensa-esp32-elf/bin/xtensa-esp32-elf-ar)
SET(CMAKE_C_FLAGS "-nostdlib -Wall -Werror -Os" CACHE STRING "" FORCE)

ADD_DEFINITIONS(-DLWIP_SOCKET)
#ADD_DEFINITIONS(-DLWIP_TIMEVAL_PRIVATE=1)
ADD_DEFINITIONS(-DESP32)
ADD_DEFINITIONS(-DICACHE_FLASH)
INCLUDE_DIRECTORIES("/home/ouyang/extdisk/github.ouyang/tiny-iot/tiny/prebuild/esp32/build/include")
INCLUDE_DIRECTORIES("/opt/ESP_IDF/components/lwip/include/lwip")
INCLUDE_DIRECTORIES("/opt/ESP_IDF/components/lwip/include/lwip/port")
INCLUDE_DIRECTORIES("/opt/ESP_IDF/components/esp32/include")
INCLUDE_DIRECTORIES("/opt/ESP_IDF/components/driver/include")
INCLUDE_DIRECTORIES("/opt/ESP_IDF/components/soc/esp32/include")
INCLUDE_DIRECTORIES("/opt/ESP_IDF/components/soc/include")
INCLUDE_DIRECTORIES("/opt/ESP_IDF/components/freertos/include")
INCLUDE_DIRECTORIES("/opt/ESP_IDF/components/heap/include")
INCLUDE_DIRECTORIES("/opt/ESP_IDF/components/vfs/include")

#----------------------------------------------------------------------------
#
# for GCC
#
#----------------------------------------------------------------------------
ADD_DEFINITIONS(-Wno-multichar)
ADD_DEFINITIONS(-Wno-pointer-to-int-cast)
ADD_DEFINITIONS(-O3)
ADD_DEFINITIONS(-fPIC)
ADD_DEFINITIONS(-std=gnu99)

#----------------------------------------------------------------------------
#
# DEBUG INFORMATION
#
#----------------------------------------------------------------------------
ADD_DEFINITIONS(-DTINY_DEBUG)
ADD_DEFINITIONS(-DTINY_STATIC)