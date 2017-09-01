CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(TinyDebug C)

SET(PROJ_NAME "TinyDebug")

#---------------------------------------------------------------------------------------
#
# include dir
#
#---------------------------------------------------------------------------------------

# Tiny
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyBase)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyPortable)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyContainer)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyLog)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyStringUtil)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyJson)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/Bson)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyMd5)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyUmm)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyThread)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyUuid)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/TinyWorker)

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
