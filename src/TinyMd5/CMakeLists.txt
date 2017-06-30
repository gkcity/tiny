#-----------------------------------------------------------------------------
#
# TinyMd5
#
#-----------------------------------------------------------------------------

SET(Header
        tiny_md5.h
    )

SET(Source
        tiny_md5.c
    )

SET(SRC
        ${Header}
        ${Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_md5_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_md5_static PROPERTIES OUTPUT_NAME "tiny_md5_static")
SET_TARGET_PROPERTIES(tiny_md5_static PROPERTIES LINKER_LANGUAGE C)