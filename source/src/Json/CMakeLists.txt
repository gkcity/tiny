#-----------------------------------------------------------------------------
#
# Json
#
#-----------------------------------------------------------------------------

SET(Json_Header
        JsonValue.h
        JsonArray.h
        JsonObject.h
        JsonDefinitions.h
        codec/JsonToken.h
        codec/JsonTokenizer.h
        codec/JsonDecoder.h
        codec/JsonEncoder.h
        value/JsonString.h
        value/JsonNumber.h
        value/JsonBoolean.h
        )

SET(Json_Source
        JsonValue.c
        JsonArray.c
        JsonObject.c
        JsonDefinitions.c
        codec/JsonToken.c
        codec/JsonTokenizer.c
        codec/JsonDecoder.c
        codec/JsonEncoder.c
        value/JsonString.c
        value/JsonNumber.c
        value/JsonBoolean.c
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
