#-----------------------------------------------------------------------------
#
# ByteBuffer
#
#-----------------------------------------------------------------------------

SET(Header
        ByteBuffer.h
    )

SET(Source
        ByteBuffer.c
    )

SET(SRC
        ${Header}
        ${Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(byte_buffer_static STATIC ${SRC})
SET_TARGET_PROPERTIES(byte_buffer_static PROPERTIES OUTPUT_NAME "byte_buffer_static")
SET_TARGET_PROPERTIES(byte_buffer_static PROPERTIES LINKER_LANGUAGE C)