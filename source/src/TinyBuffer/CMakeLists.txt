#-----------------------------------------------------------------------------
#
# TinyBuffer
#
#-----------------------------------------------------------------------------

SET(Header
        TinyBuffer.h
    )

SET(Source
        TinyBuffer.c
    )

SET(SRC
        ${Header}
        ${Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_buffer_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_buffer_static PROPERTIES OUTPUT_NAME "tiny_buffer_static")
SET_TARGET_PROPERTIES(tiny_buffer_static PROPERTIES LINKER_LANGUAGE C)