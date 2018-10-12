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
SET(CMAKE_ASM_COMPILER /opt/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcc)
SET(CMAKE_RANLIB /opt/xtensa-esp32-elf/bin/xtensa-esp32-elf-ranlib)
SET(CMAKE_AR /opt/xtensa-esp32-elf/bin/xtensa-esp32-elf-ar)
SET(CMAKE_STRIP /opt/xtensa-esp32-elf/bin/xtensa-esp32-elf-strip)
SET(CMAKE_C_FLAGS "-nostdlib -Wall -Werror -Os" CACHE STRING "" FORCE)

ADD_DEFINITIONS(-DLWIP_SOCKET)
#ADD_DEFINITIONS(-DLWIP_TIMEVAL_PRIVATE=1)
ADD_DEFINITIONS(-DESP32)
ADD_DEFINITIONS(-DICACHE_FLASH)
INCLUDE_DIRECTORIES("/home/ouyang/extdisk/github.ouyang/tiny-iot/tiny/prebuild/esp32/build/include")
INCLUDE_DIRECTORIES("/opt/ESP_IDF/components/newlib/platform_include")
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

#ADD_DEFINITIONS(-ffunction-sections)
#ADD_DEFINITIONS(-fdata-sections)
#ADD_DEFINITIONS(-fstrict-volatile-bitfields)
#ADD_DEFINITIONS(-mlongcalls)
#ADD_DEFINITIONS(-nostdlib)
#ADD_DEFINITIONS(-Wall)
#ADD_DEFINITIONS(-Werror=all)
#ADD_DEFINITIONS(-Wno-error=unused-function)
#ADD_DEFINITIONS(-Wno-error=unused-but-set-variable)
#ADD_DEFINITIONS(-Wno-error=unused-variable)
#ADD_DEFINITIONS(-Wno-error=deprecated-declarations)
#ADD_DEFINITIONS(-Wextra)
#ADD_DEFINITIONS(-Wno-unused-parameter)
#ADD_DEFINITIONS(-Wno-sign-compare)
#ADD_DEFINITIONS(-Wno-old-style-declaration)
#ADD_DEFINITIONS(-DWITH_POSIX)
#ADD_DEFINITIONS(-DESP_PLATFORM)
#ADD_DEFINITIONS(-D IDF_VER=\"v3.0-dev-960-g6cc80996\")
#ADD_DEFINITIONS(-MMD)
#ADD_DEFINITIONS(-MP)

#-ffunction-sections
#-fdata-sections
#-fstrict-volatile-bitfields
#-mlongcalls
#-nostdlib
#-Wall
#-Werror=all
#-Wno-error=unused-function
#-Wno-error=unused-but-set-variable
#-Wno-error=unused-variable
#-Wno-error=deprecated-declarations
#-Wextra
#-Wno-unused-parameter
#-Wno-sign-compare
#-Wno-old-style-declaration
#-DWITH_POSIX
#-DESP_PLATFORM
#-D IDF_VER=\"v3.0-dev-960-g6cc80996\"
#-MMD
#-MP
#-I /opt/ESP_IDF/components/app_trace/include
#-I /opt/ESP_IDF/components/app_update/include
#-I /opt/ESP_IDF/components/bootloader_support/include
#-I /opt/ESP_IDF/components/bt/include
#-I /opt/ESP_IDF/components/coap/port/include
#-I /opt/ESP_IDF/components/coap/port/include/coap
#-I /opt/ESP_IDF/components/coap/libcoap/include
#-I /opt/ESP_IDF/components/coap/libcoap/include/coap
#-I /opt/ESP_IDF/components/console/.
#-I /opt/ESP_IDF/components/cxx/include
#-I /opt/ESP_IDF/components/driver/include
#-I /opt/ESP_IDF/components/esp32/include
#-I /opt/ESP_IDF/components/esp_adc_cal/include
#-I /opt/ESP_IDF/components/ethernet/include
#-I /opt/ESP_IDF/components/expat/port/include
#-I /opt/ESP_IDF/components/expat/include/expat
#-I /opt/ESP_IDF/components/fatfs/src
#-I /opt/ESP_IDF/components/freertos/include
#-I /opt/ESP_IDF/components/heap/include
#-I /opt/ESP_IDF/components/jsmn/include/
#-I /opt/ESP_IDF/components/json/include
#-I /opt/ESP_IDF/components/json/port/include
#-I /opt/ESP_IDF/components/libsodium/libsodium/src/libsodium/include
#-I /opt/ESP_IDF/components/libsodium/port_include
#-I /opt/ESP_IDF/components/log/include
#-I /opt/ESP_IDF/components/lwip/include/lwip
#-I /opt/ESP_IDF/components/lwip/include/lwip/port
#-I /opt/ESP_IDF/components/lwip/include/lwip/posix
#-I /opt/ESP_IDF/components/lwip/apps/ping
#-I /opt/ESP_IDF/components/mbedtls/port/include
#-I /opt/ESP_IDF/components/mbedtls/include
#-I /opt/ESP_IDF/components/mdns/include
#-I /opt/ESP_IDF/components/micro-ecc/micro-ecc
#-I /opt/ESP_IDF/components/newlib/platform_include
#-I /opt/ESP_IDF/components/newlib/include
#-I /opt/ESP_IDF/components/nghttp/port/include
#-I /opt/ESP_IDF/components/nghttp/nghttp2/lib/includes
#-I /opt/ESP_IDF/components/nvs_flash/include
#-I /opt/ESP_IDF/components/openssl/include
#-I /opt/ESP_IDF/components/pthread/include
#-I /opt/ESP_IDF/components/sdmmc/include
#-I /opt/ESP_IDF/components/soc/esp32/include
#-I /opt/ESP_IDF/components/soc/include
#-I /opt/ESP_IDF/components/spi_flash/include
#-I /opt/ESP_IDF/components/spiffs/include
#-I /opt/ESP_IDF/components/tcpip_adapter/include
#-I /opt/ESP_IDF/components/ulp/include
#-I /opt/ESP_IDF/components/vfs/include
#-I /opt/ESP_IDF/components/wear_levelling/include
#-I /opt/ESP_IDF/components/wpa_supplicant/include
#-I /opt/ESP_IDF/components/wpa_supplicant/port/include
#-I /opt/ESP_IDF/components/wpa_supplicant/../esp32/include

#----------------------------------------------------------------------------
#
# DEBUG INFORMATION
#
#----------------------------------------------------------------------------
#ADD_DEFINITIONS(-DTINY_DEBUG)
ADD_DEFINITIONS(-DTINY_STATIC)

#----------------------------------------------------------------------------
#
# Export library
#
#----------------------------------------------------------------------------
SET(EXPORT_STATIC_LIBRARY 1)
SET(EXPORT_SHARED_LIBRARY 0)
