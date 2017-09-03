#-----------------------------------------------------------------------------
#
# son
#
#-----------------------------------------------------------------------------

SET(Json_Header
        JsonValue.h
        JsonArray.h
        JsonString.h
        JsonNumber.h
        JsonObject.h
    )

SET(Json_Source
        JsonValue.c
        JsonArray.c
        JsonString.c
        JsonNumber.c
        JsonObject.c
    )

SET(SRC
        ${Json_Header}
        ${Json_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(json_static STATIC ${SRC})
SET_TARGET_PROPERTIES(json_static PROPERTIES OUTPUT_NAME "json_static")
SET_TARGET_PROPERTIES(json_static PROPERTIES LINKER_LANGUAGE C)

#----------------------------------------------------------------------------
# test
#----------------------------------------------------------------------------
ADD_SUBDIRECTORY(test)
