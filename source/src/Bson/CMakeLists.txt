#-----------------------------------------------------------------------------
#
# Bson
#
#-----------------------------------------------------------------------------

SET(Bson_Header
        bson.h
    )

SET(Bson_Source
        bson.c
    )

SET(SRC
        ${Bson_Header}
        ${Bson_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_bson_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_bson_static PROPERTIES OUTPUT_NAME "tiny_bson_static")
SET_TARGET_PROPERTIES(tiny_bson_static PROPERTIES LINKER_LANGUAGE C)
