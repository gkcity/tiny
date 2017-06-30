#-----------------------------------------------------------------------------
#
# TinyUmm
#
#-----------------------------------------------------------------------------

SET(Header
        umm_malloc.h
        umm_malloc_cfg.h
    )

SET(Source
        umm_malloc.c
    )

SET(SRC
        ${Header}
        ${Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_umm_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_umm_static PROPERTIES OUTPUT_NAME "tiny_umm_static")
SET_TARGET_PROPERTIES(tiny_umm_static PROPERTIES LINKER_LANGUAGE C)