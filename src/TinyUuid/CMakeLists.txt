#-----------------------------------------------------------------------------
#
# TinyUUID
#
#-----------------------------------------------------------------------------

SET(Uuid_Header
        TinyUuid.h
    )

SET(Uuid_Source
        TinyUuid.c
    )

SET(SRC
        ${Uuid_Header}
        ${Uuid_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_uuid_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_uuid_static PROPERTIES OUTPUT_NAME "tiny_uuid_static")
SET_TARGET_PROPERTIES(tiny_uuid_static PROPERTIES LINKER_LANGUAGE C)