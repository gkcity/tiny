#-----------------------------------------------------------------------------
#
# TinyContainer
#
#-----------------------------------------------------------------------------

SET(Header
        TinyContainerListener.h
        TinyList.h
        TinyMap.h
        TinyMapItem.h
        TinyQueue.h
        )

SET(Source
        TinyList.c
        TinyMap.c
        TinyMapItem.c
        TinyQueue.c
        )

#TinyQueue.h
#TinyQueue.c

SET(SRC
        ${Header}
        ${Source}
        )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_container_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_container_static PROPERTIES OUTPUT_NAME "tiny_container_static")
SET_TARGET_PROPERTIES(tiny_container_static PROPERTIES LINKER_LANGUAGE C)

#----------------------------------------------------------------------------
# test
#----------------------------------------------------------------------------
ADD_SUBDIRECTORY(test)