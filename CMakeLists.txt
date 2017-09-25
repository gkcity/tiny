CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(Tiny C)

SET(PROJ_NAME "Tiny")

#---------------------------------------------------------------------------------------
#
# include dir
#
#---------------------------------------------------------------------------------------
SET(SOURCE_ROOT ${PROJECT_SOURCE_DIR}/source/src)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/TinyBase)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/TinyPortable)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/TinyContainer)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/TinyLog)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/TinyStringUtil)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/TinyRandom)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/Netty)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/Json)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/HttpClient)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/StringArray)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/TinyUmm)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/TinyMd5)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/TinyThread)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/TinyUuid)
INCLUDE_DIRECTORIES(${SOURCE_ROOT}/TinyWorker)

#-------------------------------------------------------------------------------------
# output dir
#-------------------------------------------------------------------------------------
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/target)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/target)
MESSAGE(STATUS "executable output:  ${EXECUTABLE_OUTPUT_PATH}")
MESSAGE(STATUS "library output: ${LIBRARY_OUTPUT_PATH}")

#-------------------------------------------------------------------------------------
#
# Source
#
#-------------------------------------------------------------------------------------

#---------------------------
# TinyBase
#---------------------------

SET(TinyBase_Header
        ${SOURCE_ROOT}/TinyBase/tiny_api.h
        ${SOURCE_ROOT}/TinyBase/tiny_base.h
        ${SOURCE_ROOT}/TinyBase/tiny_define.h
        ${SOURCE_ROOT}/TinyBase/tiny_debug.h
        ${SOURCE_ROOT}/TinyBase/tiny_ret.h
        ${SOURCE_ROOT}/TinyBase/tiny_typedef.h
        )

SET(TinyBase_Source
        ${SOURCE_ROOT}/TinyBase/tiny_ret.c
        )

#---------------------------
# TinyPortable
#---------------------------

SET(TinyPortable_Header
        ${SOURCE_ROOT}/TinyPortable/tiny_lor.h
        ${SOURCE_ROOT}/TinyPortable/tiny_malloc.h
        ${SOURCE_ROOT}/TinyPortable/tiny_strdup.h
        ${SOURCE_ROOT}/TinyPortable/tiny_snprintf.h
        ${SOURCE_ROOT}/TinyPortable/tiny_socket.h
        ${SOURCE_ROOT}/TinyPortable/tiny_inline.h
        ${SOURCE_ROOT}/TinyPortable/tiny_time.h
        )

SET(TinyPortable_Source
        ${SOURCE_ROOT}/TinyPortable/tiny_strdup.c
        ${SOURCE_ROOT}/TinyPortable/tiny_time.c
        ${SOURCE_ROOT}/TinyPortable/tiny_socket.c
        )

#---------------------------
# TinyContainer
#---------------------------

SET(TinyContainer_Header
        ${SOURCE_ROOT}/TinyContainer/TinyContainerListener.h
        ${SOURCE_ROOT}/TinyContainer/TinyList.h
        ${SOURCE_ROOT}/TinyContainer/TinyMap.h
        ${SOURCE_ROOT}/TinyContainer/TinyMapItem.h
        )

SET(TinyContainer_Source
        ${SOURCE_ROOT}/TinyContainer/TinyList.c
        ${SOURCE_ROOT}/TinyContainer/TinyMap.c
        ${SOURCE_ROOT}/TinyContainer/TinyMapItem.c
        )

#---------------------------
# TinyLog
#---------------------------

SET(TinyLog_Header
        ${SOURCE_ROOT}/TinyLog/tiny_log.h
        ${SOURCE_ROOT}/TinyLog/tiny_log_print.h
        ${SOURCE_ROOT}/TinyLog/tiny_log_binary.h
        )

SET(TinyLog_Source
        ${SOURCE_ROOT}/TinyLog/tiny_log_print.c
        ${SOURCE_ROOT}/TinyLog/tiny_log_binary.c
        )

#---------------------------
# TinyStringUtil
#---------------------------

SET(TinyStringUtil_Header
        ${SOURCE_ROOT}/TinyStringUtil/tiny_char_util.h
        ${SOURCE_ROOT}/TinyStringUtil/tiny_str_equal.h
        ${SOURCE_ROOT}/TinyStringUtil/tiny_str_get_value.h
        ${SOURCE_ROOT}/TinyStringUtil/tiny_str_split.h
        ${SOURCE_ROOT}/TinyStringUtil/tiny_url_split.h
        ${SOURCE_ROOT}/TinyStringUtil/tiny_buffer_append.h
        ${SOURCE_ROOT}/TinyStringUtil/tiny_strnlen.h
        )

SET(TinyStringUtil_Source
        ${SOURCE_ROOT}/TinyStringUtil/tiny_char_util.c
        ${SOURCE_ROOT}/TinyStringUtil/tiny_str_equal.c
        ${SOURCE_ROOT}/TinyStringUtil/tiny_str_get_value.c
        ${SOURCE_ROOT}/TinyStringUtil/tiny_str_split.c
        ${SOURCE_ROOT}/TinyStringUtil/tiny_url_split.c
        ${SOURCE_ROOT}/TinyStringUtil/tiny_buffer_append.c
        ${SOURCE_ROOT}/TinyStringUtil/tiny_strnlen.c
        )

#---------------------------
# TinyRandom
#---------------------------

SET(TinyRandom_Header
        ${SOURCE_ROOT}/TinyRandom/tiny_random.h
        )

SET(TinyRandom_Source
        ${SOURCE_ROOT}/TinyRandom/tiny_random.c
        )

#-----------------------
# Netty
#-----------------------

SET(Netty_Header
        ${SOURCE_ROOT}/Netty/selector/Selector.h
        ${SOURCE_ROOT}/Netty/channel/Channel.h
        ${SOURCE_ROOT}/Netty/channel/ChannelAddress.h
        ${SOURCE_ROOT}/Netty/channel/ChannelHandler.h
        ${SOURCE_ROOT}/Netty/channel/ChannelTimer.h
        ${SOURCE_ROOT}/Netty/channel/ChannelIdles.h
        ${SOURCE_ROOT}/Netty/channel/ChannelIdleStateHandler.h
        ${SOURCE_ROOT}/Netty/channel/ChannelInitializer.h
        ${SOURCE_ROOT}/Netty/channel/SocketChannel.h
        ${SOURCE_ROOT}/Netty/channel/stream/StreamClientChannel.h
        ${SOURCE_ROOT}/Netty/channel/stream/StreamClientChannelContext.h
        ${SOURCE_ROOT}/Netty/channel/stream/StreamServerChannel.h
        ${SOURCE_ROOT}/Netty/channel/stream/StreamServerChannelContext.h
        ${SOURCE_ROOT}/Netty/channel/multicast/MulticastChannel.h
        ${SOURCE_ROOT}/Netty/channel/multicast/MulticastChannelContext.h
        ${SOURCE_ROOT}/Netty/codec-http/HttpMessageCodec.h
        ${SOURCE_ROOT}/Netty/codec-http/HttpContent.h
        ${SOURCE_ROOT}/Netty/codec-http/HttpHeader.h
        ${SOURCE_ROOT}/Netty/codec-http/HttpMessage.h
        ${SOURCE_ROOT}/Netty/codec-http/Bytes.h
        ${SOURCE_ROOT}/Netty/bootstrap/Bootstrap.h
        ${SOURCE_ROOT}/Netty/bootstrap/LoopbackChannelHandler.h
        )

SET(Netty_Source
        ${SOURCE_ROOT}/Netty/selector/Selector.c
        ${SOURCE_ROOT}/Netty/channel/ChannelAddress.c
        ${SOURCE_ROOT}/Netty/channel/ChannelIdles.c
        ${SOURCE_ROOT}/Netty/channel/ChannelIdleStateHandler.c
        ${SOURCE_ROOT}/Netty/channel/SocketChannel.c
        ${SOURCE_ROOT}/Netty/channel/stream/StreamClientChannel.c
        ${SOURCE_ROOT}/Netty/channel/stream/StreamClientChannelContext.c
        ${SOURCE_ROOT}/Netty/channel/stream/StreamServerChannel.c
        ${SOURCE_ROOT}/Netty/channel/stream/StreamServerChannelContext.c
        ${SOURCE_ROOT}/Netty/channel/multicast/MulticastChannel.c
        ${SOURCE_ROOT}/Netty/channel/multicast/MulticastChannelContext.c
        ${SOURCE_ROOT}/Netty/codec-http/HttpMessageCodec.c
        ${SOURCE_ROOT}/Netty/codec-http/HttpContent.c
        ${SOURCE_ROOT}/Netty/codec-http/HttpHeader.c
        ${SOURCE_ROOT}/Netty/codec-http/HttpMessage.c
        ${SOURCE_ROOT}/Netty/codec-http/Bytes.c
        ${SOURCE_ROOT}/Netty/bootstrap/Bootstrap.c
        ${SOURCE_ROOT}/Netty/bootstrap/LoopbackChannelHandler.c
        )

#---------------------------
# Json
#---------------------------

SET(Json_Header
        ${SOURCE_ROOT}/Json/JsonObject.h
        ${SOURCE_ROOT}/Json/JsonArray.h
        ${SOURCE_ROOT}/Json/JsonValue.h
        ${SOURCE_ROOT}/Json/codec/JsonDecoder.h
        ${SOURCE_ROOT}/Json/codec/JsonEncoder.h
        ${SOURCE_ROOT}/Json/codec/JsonToken.h
        ${SOURCE_ROOT}/Json/value/JsonNumber.h
        ${SOURCE_ROOT}/Json/value/JsonBoolean.h
        ${SOURCE_ROOT}/Json/value/JsonString.h
        )

SET(Json_Source
        ${SOURCE_ROOT}/Json/JsonObject.c
        ${SOURCE_ROOT}/Json/JsonArray.c
        ${SOURCE_ROOT}/Json/JsonValue.c
        ${SOURCE_ROOT}/Json/codec/JsonDecoder.c
        ${SOURCE_ROOT}/Json/codec/JsonEncoder.c
        ${SOURCE_ROOT}/Json/codec/JsonToken.c
        ${SOURCE_ROOT}/Json/value/JsonString.c
        ${SOURCE_ROOT}/Json/value/JsonNumber.c
        ${SOURCE_ROOT}/Json/value/JsonBoolean.c
        )

#---------------------------
# HttpClient
#---------------------------

SET(HttpClient_Header
        ${SOURCE_ROOT}/HttpClient/HttpClient.h
        ${SOURCE_ROOT}/HttpClient/HttpClientHandler.h
        ${SOURCE_ROOT}/HttpClient/HttpExchange.h
        )

SET(HttpClient_Source
        ${SOURCE_ROOT}/HttpClient/HttpClient.c
        ${SOURCE_ROOT}/HttpClient/HttpClientHandler.c
        ${SOURCE_ROOT}/HttpClient/HttpExchange.c
        )

#---------------------------
# StringArray
#---------------------------

SET(StringArray_Header
        ${SOURCE_ROOT}/StringArray/StringArray.h
        )

SET(StringArray_Source
        ${SOURCE_ROOT}/StringArray/StringArray.c
        )

#---------------------------
# TinyThread
#---------------------------

SET(TinyThread_Header
        ${SOURCE_ROOT}/TinyThread/TinyCondition.h
        ${SOURCE_ROOT}/TinyThread/TinyMutex.h
        ${SOURCE_ROOT}/TinyThread/TinySemaphore.h
        ${SOURCE_ROOT}/TinyThread/TinyThread.h
        )

SET(TinyThread_Source
        ${SOURCE_ROOT}/TinyThread/TinyCondition.c
        ${SOURCE_ROOT}/TinyThread/TinyMutex.c
        ${SOURCE_ROOT}/TinyThread/TinySemaphore.c
        ${SOURCE_ROOT}/TinyThread/TinyThread.c
        )

SOURCE_GROUP(TinyBase\\headers FILES ${TinyBase_Header})
SOURCE_GROUP(TinyBase\\sources FILES ${TinyBase_Source})
SOURCE_GROUP(TinyPortable\\headers FILES ${TinyPortable_Header})
SOURCE_GROUP(TinyPortable\\sources FILES ${TinyPortable_Source})
SOURCE_GROUP(TinyContainer\\headers FILES ${TinyContainer_Header})
SOURCE_GROUP(TinyContainer\\sources FILES ${TinyContainer_Source})
SOURCE_GROUP(TinyLog\\headers FILES ${TinyLog_Header})
SOURCE_GROUP(TinyLog\\sources FILES ${TinyLog_Source})
SOURCE_GROUP(TinyStringUtil\\headers FILES ${TinyStringUtil_Header})
SOURCE_GROUP(TinyStringUtil\\sources FILES ${TinyStringUtil_Source})
SOURCE_GROUP(Netty\\headers FILES ${Netty_Header})
SOURCE_GROUP(Netty\\sources FILES ${Netty_Source})
SOURCE_GROUP(Json\\headers FILES ${Json_Header})
SOURCE_GROUP(Json\\sources FILES ${Json_Source})
SOURCE_GROUP(HttpClient\\headers FILES ${HttpClient_Header})
SOURCE_GROUP(HttpClient\\sources FILES ${HttpClient_Source})
SOURCE_GROUP(StringArray\\headers FILES ${StringArray_Header})
SOURCE_GROUP(StringArray\\sources FILES ${StringArray_Source})
SOURCE_GROUP(TinyThread\\headers FILES ${TinyThread_Header})
SOURCE_GROUP(TinyThread\\sources FILES ${TinyThread_Source})

IF (TINY_THREAD_ENABLED EQUAL 1)
    MESSAGE(STATUS "TinyThread enabled")
    SET(SRC
            ${TinyBase_Header}
            ${TinyBase_Source}
            ${TinyPortable_Header}
            ${TinyPortable_Source}
            ${TinyContainer_Header}
            ${TinyContainer_Source}
            ${TinyLog_Header}
            ${TinyLog_Source}
            ${TinyStringUtil_Header}
            ${TinyStringUtil_Source}
            ${TinyRandom_Header}
            ${TinyRandom_Source}
            ${Netty_Header}
            ${Netty_Source}
            ${Json_Header}
            ${Json_Source}
            ${HttpClient_Header}
            ${HttpClient_Source}
            ${StringArray_Header}
            ${StringArray_Source}
            ${TinyThread_Header}
            ${TinyThread_Source}
            )
ELSE ()
    MESSAGE(STATUS "TinyThread disabled")
    SET(SRC
            ${TinyBase_Header}
            ${TinyBase_Source}
            ${TinyPortable_Header}
            ${TinyPortable_Source}
            ${TinyContainer_Header}
            ${TinyContainer_Source}
            ${TinyLog_Header}
            ${TinyLog_Source}
            ${TinyStringUtil_Header}
            ${TinyStringUtil_Source}
            ${TinyRandom_Header}
            ${TinyRandom_Source}
            ${Netty_Header}
            ${Netty_Source}
            ${Json_Header}
            ${Json_Source}
            ${HttpClient_Header}
            ${HttpClient_Source}
            ${StringArray_Header}
            ${StringArray_Source}
            )
ENDIF ()

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_static PROPERTIES OUTPUT_NAME "tiny_static")
SET_TARGET_PROPERTIES(tiny_static PROPERTIES LINKER_LANGUAGE C)

#----------------------------------------------------------------------------
# shared library
#----------------------------------------------------------------------------
MESSAGE(STATUS "link library: ${DEPENDENCE_LIB}")
ADD_LIBRARY(tiny SHARED ${SRC})
SET_TARGET_PROPERTIES(tiny PROPERTIES OUTPUT_NAME "tiny")
SET_TARGET_PROPERTIES(tiny PROPERTIES LINKER_LANGUAGE C)
TARGET_LINK_LIBRARIES(tiny ${DEPENDENCE_LIB})
