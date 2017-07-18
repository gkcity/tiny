#-----------------------------------------------------------------------------
#
# miot
#
#-----------------------------------------------------------------------------

#INCLUDE_DIRECTORIES(../Netty
#	../TinyBase
#	../TinyContainer
#	../TinyJson
#	../TinyLog
#	../TinyMd5
#	../TinyPortable
#	../TinyStringUtil
#	../TinyThread
#	../TinyUmm
#	../TinyUuid
#	../TinyWorker
#)

SET(Header
	aes256.h
	aes.h
	cdecode.h
	cencode.h
	LibMd5.h
	LibSha1.h
	LibSha256.h
	miio.h
	miio_util.h
	miot.h
	ot_cloud.h
	ot_handler.h
	ot_packet.h
	ot_packet_handler.h
    )

SET(Source
	aes256.c
	aes.c
	cdecode.c
	cencode.c
	LibMd5.c
	LibSha1.c
	LibSha256.c
	miio_util.c
	miot.c
	ot_cloud.c
	ot_handler.c
	ot_packet.c
	ot_packet_handler.c
    )

SET(SRC
        ${Header}
        ${Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(miot_static STATIC ${SRC})
SET_TARGET_PROPERTIES(miot_static PROPERTIES OUTPUT_NAME "miot_static")
SET_TARGET_PROPERTIES(miot_static PROPERTIES LINKER_LANGUAGE C)

#----------------------------------------------------------------------------
# shared library
#----------------------------------------------------------------------------
#ADD_LIBRARY(miot_shared SHARED ${SRC})
#SET_TARGET_PROPERTIES(miot_shared PROPERTIES OUTPUT_NAME "miot")
#SET_TARGET_PROPERTIES(miot_shared PROPERTIES LINKER_LANGUAGE C)
#TARGET_LINK_LIBRARIES(miot_shared netty_shared tiny_shared ${OS_LIB})

#----------------------------------------------------------------------------
# test
#----------------------------------------------------------------------------
ADD_SUBDIRECTORY(test)
