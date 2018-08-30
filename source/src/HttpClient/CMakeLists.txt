#-----------------------------------------------------------------------------
#
# HttpClient
#
#-----------------------------------------------------------------------------

SET(HttpClient_Header
        HttpClient.h
        HttpExchange.h
        )

SET(HttpClient_Source
        HttpClient.c
        HttpClientHandler.c
        HttpExchange.c
        )

SOURCE_GROUP(HttpClient\\headers        FILES     ${HttpClient_Header})
SOURCE_GROUP(HttpClient\\sources        FILES     ${HttpClient_Source})

SET(SRC
        ${HttpClient_Header}
        ${HttpClient_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(http_client_static STATIC ${SRC})
SET_TARGET_PROPERTIES(http_client_static PROPERTIES OUTPUT_NAME "http_client_static")
SET_TARGET_PROPERTIES(http_client_static PROPERTIES LINKER_LANGUAGE C)

#----------------------------------------------------------------------------
# example
#----------------------------------------------------------------------------
ADD_SUBDIRECTORY(example)