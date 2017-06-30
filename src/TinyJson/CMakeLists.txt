#-----------------------------------------------------------------------------
#
# TinyJson
#
#-----------------------------------------------------------------------------

SET(TinyJson_Header
        json.h
    )

SET(TinyJson_Source
        json.c
    )

SET(SRC
        ${TinyJson_Header}
        ${TinyJson_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_json_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_json_static PROPERTIES OUTPUT_NAME "tiny_json_static")
SET_TARGET_PROPERTIES(tiny_json_static PROPERTIES LINKER_LANGUAGE C)