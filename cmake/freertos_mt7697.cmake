MESSAGE(STATUS "make for mt7697")
SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_SYSTEM_PROCESSOR MT7697)
SET(CMAKE_FIND_ROOT_PATH "/opt/linkit_sdk_v4.6.0_public/tools/gcc/gcc-arm-none-eabi")
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

SET(CMAKE_C_COMPILER /opt/linkit_sdk_v4.6.0_public/tools/gcc/gcc-arm-none-eabi/bin/arm-none-eabi-gcc)
#ADD_COMPILE_OPTIONS("--specs=nano.specs -lnosys -nostartfiles -Wl,-wrap=malloc -Wl,-wrap=calloc -Wl,-wrap=realloc -Wl,-wrap=free -Wl,-Tmt7687_flash.ld -Wl,--gc-sections")
SET(CMAKE_SHARED_LINKER_FLAGS "--specs=nano.specs -lnosys -nostartfiles -Wl,-wrap=malloc -Wl,-wrap=calloc -Wl,-wrap=realloc -Wl,-wrap=free -Wl,-Tmt7687_flash.ld -Wl,--gc-sections" CACHE INTERNAL "")
SET(CMAKE_STATIC_LINKER_FLAGS "--specs=nano.specs -lnosys -nostartfiles -Wl,-wrap=malloc -Wl,-wrap=calloc -Wl,-wrap=realloc -Wl,-wrap=free -Wl,-Tmt7687_flash.ld -Wl,--gc-sections" CACHE INTERNAL "")
SET(CMAKE_EXE_LINKER_FLAGS "--specs=nano.specs -lnosys -nostartfiles -DMTK_NVDM_ENABLE -std=gnu99 -mlittle-endian -mthumb -mcpu=cortex-m4 -Wdouble-promotion -mfpu=fpv4-sp-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections -fno-builtin -gdwarf-2 -Os -fno-strict-aliasing -fno-common -Wall -Wimplicit-function-declaration -Werror=uninitialized -Wno-error=maybe-uninitialized -Werror=return-type -Wno-switch -DPCFG_OS=2 -D_REENT_SMALL -DPRODUCT_VERSION=7697 -DMTK_FLASH_DIRECT_DL -DMTK_PATCH_DL_ENABLE -DMTK_BSPEXT_ENABLE -DMTK_DEBUG_LEVEL_DEBUG -DMTK_DEBUG_LEVEL_INFO -DMTK_DEBUG_LEVEL_WARNING -DMTK_DEBUG_LEVEL_ERROR -DMTK_HAL_LOWPOWER_ENABLE -DMTK_HIF_GDMA_ENABLE -DMTK_IPERF_ENABLE -DMTK_LWIP_ENABLE")
#SET(CMAKE_EXE_LINKER_FLAGS "--specs=nano.specs -lnosys -nostartfiles")
SET(CMAKE_CXX_COMPILER /opt/linkit_sdk_v4.6.0_public/tools/gcc/gcc-arm-none-eabi/bin/arm-none-eabi-g++)
SET(CMAKE_ASM_COMPILER /opt/linkit_sdk_v4.6.0_public/tools/gcc/gcc-arm-none-eabi/bin/arm-none-eabi-as)
SET(CMAKE_RANLIB /opt/linkit_sdk_v4.6.0_public/tools/gcc/gcc-arm-none-eabi/bin/arm-none-eabi-ranlib)
SET(CMAKE_AR /opt/linkit_sdk_v4.6.0_public/tools/gcc/gcc-arm-none-eabi/bin/arm-none-eabi-ar)
SET(CMAKE_STRIP /opt/linkit_sdk_v4.6.0_public/tools/gcc/gcc-arm-none-eabi/bin/arm-none-eabi-strip)
#SET(CMAKE_C_FLAGS "-nostdlib -Wall -Werror -Os" CACHE STRING "" FORCE)

ADD_DEFINITIONS(-DLWIP_SOCKET)
ADD_DEFINITIONS(-DLWIP_TIMEVAL_PRIVATE=1)
ADD_DEFINITIONS(-DMT7697)
INCLUDE_DIRECTORIES("/opt/linkit_sdk_v4.6.0_public/middleware/third_party/lwip/src/include/lwip")
INCLUDE_DIRECTORIES("/opt/linkit_sdk_v4.6.0_public/middleware/third_party/lwip/ports/include")

#----------------------------------------------------------------------------
#
# for GCC
#
#----------------------------------------------------------------------------
ADD_DEFINITIONS(-Wno-multichar)
ADD_DEFINITIONS(-Wno-pointer-to-int-cast)
ADD_DEFINITIONS(-fPIC)
ADD_DEFINITIONS(-std=gnu99)
#ADD_DEFINITIONS(-DMTK_NVDM_ENABLE)
#ADD_DEFINITIONS(-mlittle-endian)
#ADD_DEFINITIONS(-mthumb)
#ADD_DEFINITIONS(-mcpu=cortex-m4)
#ADD_DEFINITIONS(-Wdouble-promotion)
#ADD_DEFINITIONS(-mfpu=fpv4-sp-d16)
#ADD_DEFINITIONS(-mfloat-abi=hard)
#ADD_DEFINITIONS(-Wdouble-promotion)
#ADD_DEFINITIONS(-ffunction-sections)
#ADD_DEFINITIONS(-fdata-sections)
#ADD_DEFINITIONS(-fno-builtin)
#ADD_DEFINITIONS(-gdwarf-2)
#ADD_DEFINITIONS(-Os)
#ADD_DEFINITIONS(-fno-strict-aliasing)
#ADD_DEFINITIONS(-fno-common)
#ADD_DEFINITIONS(-Wall)
#ADD_DEFINITIONS(-Wimplicit-function-declaration)
#ADD_DEFINITIONS(-Werror=uninitialized)
#ADD_DEFINITIONS(-Wno-error=maybe-uninitialized)
#ADD_DEFINITIONS(-Werror=return-type)
#ADD_DEFINITIONS(-Wno-switch)
#ADD_DEFINITIONS(-DPCFG_OS=2)
#ADD_DEFINITIONS(-D_REENT_SMALL)
#ADD_DEFINITIONS(-DPRODUCT_VERSION=7697)
#ADD_DEFINITIONS(-DMTK_FLASH_DIRECT_DL)
#ADD_DEFINITIONS(-DMTK_PATCH_DL_ENABLE)
#ADD_DEFINITIONS(-DMTK_BSPEXT_ENABLE)
#ADD_DEFINITIONS(-DMTK_DEBUG_LEVEL_DEBUG)
#ADD_DEFINITIONS(-DMTK_DEBUG_LEVEL_INFO)
#ADD_DEFINITIONS(-DMTK_DEBUG_LEVEL_WARNING)
#ADD_DEFINITIONS(-DMTK_DEBUG_LEVEL_ERROR)
#ADD_DEFINITIONS(-DMTK_HAL_LOWPOWER_ENABLE)
#ADD_DEFINITIONS(-DMTK_HIF_GDMA_ENABLE)
#ADD_DEFINITIONS(-DMTK_IPERF_ENABLE)
#ADD_DEFINITIONS(-DMTK_LWIP_ENABLE)
#ADD_DEFINITIONS(-DMBEDTLS_CONFIG_FILE=\"config-mtk-basic.h\")
#ADD_DEFINITIONS(-DMTK_MINISUPP_ENABLE)
#ADD_DEFINITIONS(-DMTK_PING_OUT_ENABLE)
#ADD_DEFINITIONS(-DMTK_WIFI_TGN_VERIFY_ENABLE)
#ADD_DEFINITIONS(-Wdouble-promotion)
#ADD_DEFINITIONS(-mfpu=fpv4-sp-d16)
#ADD_DEFINITIONS(-mfloat-abi=hard)
#ADD_DEFINITIONS(-DUSE_HAL_DRIVER)
#ADD_DEFINITIONS(-DDATA_PATH_87)
#ADD_DEFINITIONS(-DMTK_EXTERNAL_FLASH_ENABLE)
#ADD_DEFINITIONS(-DFREERTOS_ENABLE)
#ADD_DEFINITIONS(-DMTK_NVDM_ENABLE)


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
