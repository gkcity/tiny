#-----------------------------------------------------------------------------
#
# TinyRandom
#
#-----------------------------------------------------------------------------

SET(TinyRandom_Header
        tiny_random.h
        )

SET(TinyRandom_Source
        tiny_random.c
        )

SET(SRC
        ${TinyRandom_Header}
        ${TinyRandom_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_random_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_random_static PROPERTIES OUTPUT_NAME "tiny_random_static")
SET_TARGET_PROPERTIES(tiny_random_static PROPERTIES LINKER_LANGUAGE C)