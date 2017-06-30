#-----------------------------------------------------------------------------
#
# TinyBase
#
#-----------------------------------------------------------------------------

SET(Header
        tiny_api.h
        tiny_base.h
        tiny_define.h
        tiny_debug.h
        tiny_ret.h
        tiny_typedef.h
    )

SET(Source
        tiny_ret.c
    )

SET(SRC
        ${Header}
        ${Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_base_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_base_static PROPERTIES OUTPUT_NAME "tiny_base_static")
SET_TARGET_PROPERTIES(tiny_base_static PROPERTIES LINKER_LANGUAGE C)