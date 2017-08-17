#-----------------------------------------------------------------------------
#
# TinyPortable
#
#-----------------------------------------------------------------------------

SET(Header
        tiny_malloc.h
        tiny_strdup.h
        tiny_snprintf.h
        tiny_socket.h
        tiny_time.h
    )

SET(Source
        tiny_malloc.c
        tiny_strdup.c
        tiny_time.c
        )

SET(SRC
        ${Header}
        ${Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_portable_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_portable_static PROPERTIES OUTPUT_NAME "tiny_portable_static")
SET_TARGET_PROPERTIES(tiny_portable_static PROPERTIES LINKER_LANGUAGE C)