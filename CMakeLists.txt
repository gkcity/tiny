CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(TINY C)

SET(PROJ_NAME "Tiny")

#---------------------------------------------------------------------------------------
#
# c flags
#
#---------------------------------------------------------------------------------------
#ADD_DEFINITIONS(-DTINY_UMM)
ADD_DEFINITIONS(-DTINY_DEBUG)
ADD_DEFINITIONS(-DTINY_STATIC)
ADD_DEFINITIONS(-DNETTY_STATIC)

#---------------------------------------------------------------------------------------
# WIN32
#---------------------------------------------------------------------------------------
IF(WIN32)

    MESSAGE(STATUS "build ${PROJ_NAME}for Windows")

    ADD_DEFINITIONS(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-DUNICODE)
    ADD_DEFINITIONS(-D_UNICODE)

ELSE(WIN32)

    ADD_DEFINITIONS(-Wno-multichar)

    ADD_DEFINITIONS(-Wno-pointer-to-int-cast)

    # release, macos only link by O2 & O3
#    ADD_DEFINITIONS(-O0)
#    ADD_DEFINITIONS(-O1)
#    ADD_DEFINITIONS(-O2)
    ADD_DEFINITIONS(-O3)

    # debug
    ADD_DEFINITIONS(-g)

    ADD_DEFINITIONS(-fPIC)
    ADD_DEFINITIONS(-std=c99)

    # for ARM ?
    #ADD_DEFINITIONS(-D__arm__)

    #-----------------------------------------------------------------------------------
    # APPLE MACOSX
    #-----------------------------------------------------------------------------------
    IF(APPLE)
        MESSAGE(STATUS "build ${PROJ_NAME} for osx")
        ADD_DEFINITIONS(-D__MAC_OSX__)

        # Apple LLVM Compiler
        # SET(CMAKE_CXX_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++)

        # LLVM gcc Compiler
        SET(CMAKE_CXX_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++)
        SET(CMAKE_CXX_FLAGS -m64)

        #SET(CMAKE_CXX_COMPILER g++)
        #SET(CMAKE_CXX_FLAGS -m64)

    #-----------------------------------------------------------------------------------
    # Linux
    #-----------------------------------------------------------------------------------
    ELSE(APPLE)
        MESSAGE(STATUS "build ${PROJ_NAME} for Linux")
        ADD_DEFINITIONS(-D__LINUX__)
        ADD_DEFINITIONS(-D_POSIX_C_SOURCE=200112L)
    ENDIF(APPLE)
ENDIF(WIN32)

#---------------------------------------------------------------------------------------
#
# include dir
#
#---------------------------------------------------------------------------------------

# Tiny
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyBase)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyContainer)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyLog)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyMd5)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyMemory)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyStringUtil)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyThread)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyUuid)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyWorker)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyPortable)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyJson)

# Netty
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Netty)

#--------------------------------------------------------------------------------------
#
# lib
#
#--------------------------------------------------------------------------------------
IF (WIN32)
    SET(OS_LIB ws2_32.lib)
ELSE (WIN32)
    IF(LINUX)
        SET(OS_LIB rt)
    ENDIF(LINUX)
ENDIF (WIN32)

MESSAGE(STATUS "os_lib: ${OS_LIB}")

#-------------------------------------------------------------------------------------
# set output dir
#-------------------------------------------------------------------------------------
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)
MESSAGE(STATUS "executable output:  ${EXECUTABLE_OUTPUT_PATH}")
MESSAGE(STATUS "library output: ${LIBRARY_OUTPUT_PATH}")

#-------------------------------------------------------------------------------------
#
# project
#
#-------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(src)
