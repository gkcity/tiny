#-----------------------------------------------------------------------------
#
# TcpClient
#
#-----------------------------------------------------------------------------

SET(TcpClient_Header
        TcpClient.h
        TcpClientHandler.h
        TcpExchange.h
        )

SET(TcpClient_Source
        TcpClient.c
        TcpClientHandler.c
        TcpExchange.c
        )

SOURCE_GROUP(TcpClient\\headers        FILES     ${TcpClient_Header})
SOURCE_GROUP(TcpClient\\sources        FILES     ${TcpClient_Source})

SET(SRC
        ${TcpClient_Header}
        ${TcpClient_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tcp_client_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tcp_client_static PROPERTIES OUTPUT_NAME "tcp_client_static")
SET_TARGET_PROPERTIES(tcp_client_static PROPERTIES LINKER_LANGUAGE C)

#----------------------------------------------------------------------------
# example
#----------------------------------------------------------------------------
ADD_SUBDIRECTORY(example)