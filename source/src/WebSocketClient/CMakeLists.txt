#-----------------------------------------------------------------------------
#
# WebSocket Client
#
#-----------------------------------------------------------------------------

SET(WsClient_Header
        WsClient.h
        WsClientHandler.h
        WebSocketFrame.h
        )

SET(WsClient_Source
        WsClient.c
        WsClientHandler.c
        )

SOURCE_GROUP(WsClient\\headers        FILES     ${WsClient_Header})
SOURCE_GROUP(WsClient\\sources        FILES     ${WsClient_Source})

SET(SRC
        ${WsClient_Header}
        ${WsClient_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(ws_client_static STATIC ${SRC})
SET_TARGET_PROPERTIES(ws_client_static PROPERTIES OUTPUT_NAME "ws_client_static")
SET_TARGET_PROPERTIES(ws_client_static PROPERTIES LINKER_LANGUAGE C)

#----------------------------------------------------------------------------
# example
#----------------------------------------------------------------------------
ADD_SUBDIRECTORY(example)