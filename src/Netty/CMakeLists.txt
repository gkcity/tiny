#-----------------------------------------------------------------------------
#
# Netty
#
#-----------------------------------------------------------------------------

#-----------------------
# Common
#-----------------------
SET(Common_Header
        common/Netty_api.h
    )

#-----------------------
# Selector
#-----------------------
SET(Selector_Header
        selector/Selector.h
    )

SET(Selector_Source
        selector/Selector.c
    )

#-----------------------
# channel
#-----------------------
SET(Channel_Header
        channel/Channel.h
        channel/ChannelHandler.h
        channel/ChannelInitializer.h
        channel/ChannelIdleStateHandler.h
        channel/SocketChannel.h
        channel/stream/StreamServerChannel.h
        channel/stream/StreamServerChannelContext.h
        channel/stream/StreamClientChannel.h
        channel/stream/StreamClientChannelContext.h
        channel/multicast/MulticastChannel.h
        channel/multicast/MulticastChannelContext.h
        )

SET(Channel_Source
        channel/Channel.c
        channel/ChannelIdleStateHandler.c
        channel/SocketChannel.c
        channel/stream/StreamServerChannel.c
        channel/stream/StreamServerChannelContext.c
        channel/stream/StreamClientChannel.c
        channel/stream/StreamClientChannelContext.c
        channel/multicast/MulticastChannel.c
        channel/multicast/MulticastChannelContext.c
    )

#-----------------------
# timer
#-----------------------
SET(Timer_Header
        timer/Timer.h
    )

SET(Timer_Source
        timer/Timer.c
    )

#-----------------------
# codec-http
#-----------------------
SET(Codec_http_Header
        codec-http/HttpMessageCodec.h
        codec-http/HttpContent.h
        codec-http/HttpHeader.h
        codec-http/HttpMessage.h
    )

SET(Codec_http_Source
        codec-http/HttpMessageCodec.c
        codec-http/HttpContent.c
        codec-http/HttpHeader.c
        codec-http/HttpMessage.c
    )

#-----------------------
# ByteBuf
#------------------------
Set(ByteBuf_Header
	ByteBuf/byte_buf.h
)

Set(ByteBuf_Source
	ByteBuf/byte_buf.c
)

#-----------------------
# bootstrap
#-----------------------
SET(Bootstrap_Header
        bootstrap/Bootstrap.h
    )

SET(Bootstrap_Source
        bootstrap/Bootstrap.c
    )

#----------------------------------------------------------------------------
# source
#----------------------------------------------------------------------------
SET(SRC
        ${Common_Header}
        ${Selector_Header}
        ${Selector_Source}
        ${Channel_Header}
        ${Channel_Source}
        ${Timer_Header}
        ${Timer_Source}
        ${Codec_http_Header}
        ${Codec_http_Source}
        ${ByteBuf_Header}
        ${ByteBuf_Source}
        ${Bootstrap_Header}
        ${Bootstrap_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(netty_static STATIC ${SRC})
SET_TARGET_PROPERTIES(netty_static PROPERTIES OUTPUT_NAME "netty_static")
SET_TARGET_PROPERTIES(netty_static PROPERTIES LINKER_LANGUAGE C)

#----------------------------------------------------------------------------
# shared library
#----------------------------------------------------------------------------
#ADD_LIBRARY(netty_shared SHARED ${SRC})
#SET_TARGET_PROPERTIES(netty_shared PROPERTIES OUTPUT_NAME "netty")
#SET_TARGET_PROPERTIES(netty_shared PROPERTIES LINKER_LANGUAGE C)
#TARGET_LINK_LIBRARIES(netty_shared tiny_shared ${OS_LIB})

#----------------------------------------------------------------------------
# test
#----------------------------------------------------------------------------
#ADD_SUBDIRECTORY(test)

#----------------------------------------------------------------------------
# example
#----------------------------------------------------------------------------
#ADD_SUBDIRECTORY(example-httpserver)
#ADD_SUBDIRECTORY(example-timer)
#ADD_SUBDIRECTORY(example-ssdp)