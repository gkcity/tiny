MESSAGE(STATUS "make for MacOS")
SET(CMAKE_SYSTEM_NAME Darwin)
SET(CMAKE_SYSTEM_VERSION 1)
SET(CMAKE_SYSTEM_PROCESSOR amd64)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#----------------------------------------------------------------------------
#
# for GCC
#
#----------------------------------------------------------------------------
ADD_DEFINITIONS(-Wno-multichar)
ADD_DEFINITIONS(-Wno-pointer-to-int-cast)
ADD_DEFINITIONS(-O2)
ADD_DEFINITIONS(-fPIC)
ADD_DEFINITIONS(-std=c99)
ADD_DEFINITIONS(-D__MACOS__)

#----------------------------------------------------------------------------
#
# DEBUG INFORMATION
#
#----------------------------------------------------------------------------
ADD_DEFINITIONS(-DTINY_DEBUG)