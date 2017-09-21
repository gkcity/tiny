#-----------------------------------------------------------------------------
#
# Bson
#
#-----------------------------------------------------------------------------

SET(Bson_Header
        BsonValue.h
        BsonArray.h
        BsonObject.h
        codec/BsonDecoder.h
        value/BsonBinary.h
        value/BsonBoolean.h
        value/BsonCstring.h
        value/BsonDateTime.h
        value/BsonDecimal128.h
        value/BsonFloatingPoint.h
        value/BsonInt32.h
        value/BsonInt64.h
        value/BsonJavascriptCode.h
        value/BsonJavascriptCodeScope.h
        value/BsonMaxKey.h
        value/BsonMinKey.h
        value/BsonString.h
        value/BsonTimestamp.h
        )

SET(Bson_Source
        BsonValue.c
        BsonArray.c
        BsonObject.c
        codec/BsonDecoder.c
        value/BsonBinary.c
        value/BsonBoolean.c
        value/BsonCstring.c
        value/BsonDateTime.c
        value/BsonDecimal128.c
        value/BsonFloatingPoint.c
        value/BsonInt32.c
        value/BsonInt64.c
        value/BsonJavascriptCode.c
        value/BsonJavascriptCodeScope.c
        value/BsonMaxKey.c
        value/BsonMinKey.c
        value/BsonString.c
        value/BsonTimestamp.c
        )

SET(SRC
        ${Bson_Header}
        ${Bson_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(bson_static STATIC ${SRC})
SET_TARGET_PROPERTIES(bson_static PROPERTIES OUTPUT_NAME "bson_static")
SET_TARGET_PROPERTIES(bson_static PROPERTIES LINKER_LANGUAGE C)

#----------------------------------------------------------------------------
# test
#----------------------------------------------------------------------------
ADD_SUBDIRECTORY(test)
