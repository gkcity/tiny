/**
 * Copyright (C) 2013-2015
 *
 * @author wenzhenwei@xiaomi.com
 * @date   2013-11-19
 *
 * @file   StreamClientChannel.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <tiny_inet.h>
#include <tiny_socket.h>
#include "tiny_snprintf.h"
#include "StreamClientChannel.h"
#include "StreamClientChannelContext.h"
#include "channel/SocketChannel.h"

#define TAG     "StreamClientChannel"


bool StreamClientChannel_isConnected(Channel* thiz)
 {
     return thiz->fd != 0;
 }

static void StreamClientChannel_Dispose(Channel *thiz)
{
    if (thiz->ctx != NULL)
    {
        StreamClientChannelContext_Delete((StreamClientChannelContext *)thiz->ctx);
        thiz->ctx = NULL;
    }
    if (StreamClientChannel_isConnected(thiz))
    {
    	StreamClientChannel_Close(thiz);
    }
}

void StreamClientChannel_Delete(Channel *thiz)
{
    StreamClientChannel_Dispose(thiz);
    tiny_free(thiz);
}

static void StreamClientChannel_OnRegister2(Channel *thiz, Selector *selector)
{
    if (Channel_IsActive(thiz))
    {
        Selector_Register(selector, thiz->fd, SELECTOR_OP_ALL);
        LOG_D(TAG, "StreamClientChannel_OnRegister2: %d", thiz->fd);
    }
}

static void StreamClientChannel_OnRegister(Channel *thiz, Selector *selector)
{
    if (Channel_IsActive(thiz))
    {
        Selector_Register(selector, thiz->fd, SELECTOR_OP_READ);
        LOG_D(TAG, "StreamClientChannel_OnRegister: %d", thiz->fd);
    }
}

static void StreamClientChannel_OnRemove(Channel *thiz)
{
    LOG_D(TAG, "StreamClientChannel_OnRemove");

    StreamClientChannel_Delete(thiz);
}


static TinyRet StreamClientChannel_OnRead(Channel *thiz, Selector *selector)
{
    StreamClientChannelContext *ctx = (StreamClientChannelContext *)thiz->ctx;
    int error;
    socklen_t len = sizeof(error);
    LOG_D(TAG, "connect receive read");

    if (Selector_IsReadable(selector, thiz->fd))
    {
        if (getsockopt(thiz->fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
            LOG_E(TAG, "connect error: %d", error);
            return TINY_RET_E_INTERNAL;
        }
        thiz->onActive(thiz);
    }

    return TINY_RET_OK;
}

static TinyRet StreamClientChannel_OnWrite(Channel* thiz, Selector* selector)
{
    StreamClientChannelContext *ctx = (StreamClientChannelContext *)thiz->ctx;
    int error;
    socklen_t len = sizeof(error);
    LOG_D(TAG, "connect receive write");

    if (Selector_IsWriteable(selector, thiz->fd))
    {
        if (getsockopt(thiz->fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
            LOG_E(TAG, "connect error: %d", error);
            return TINY_RET_E_INTERNAL;
        }
        thiz->onActive(thiz);
    }

    return TINY_RET_OK;
}
/*
static void StreamClientChannel_OnInactive(Channel *thiz)
{
    LOG_D(TAG, "StreamClientChannel_OnInactive");
    thiz->onInactive(thiz);
    Channel_Close(thiz);
}*/

static void StreamClientChannel_OnActive(Channel* thiz)
{
    RETURN_IF_FAIL(thiz);

    LOG_D(TAG, "connected");

    if (thiz->ctx != NULL)
    {
    	((StreamClientChannelContext *)thiz->ctx) ->initializer(thiz, ((StreamClientChannelContext *)thiz->ctx) ->initializerContext);
    }
    thiz->onActive=SocketChannel_OnActive;
    thiz->onRead = SocketChannel_OnRead;
    thiz->onRegister = StreamClientChannel_OnRegister;

    thiz->onActive(thiz);
}

static TinyRet StreamClientChannel_Construct(Channel *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        thiz->onRegister = StreamClientChannel_OnRegister2;
        thiz->onRemove = StreamClientChannel_OnRemove;
        //thiz->onInactive = StreamClientChannel_OnInactive;
        thiz->onActive = StreamClientChannel_OnActive;
        thiz->onRead = StreamClientChannel_OnRead;
        thiz->onWrite = StreamClientChannel_OnWrite;

        thiz->ctx = StreamClientChannelContext_New();
        if (thiz->ctx == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }
    } while (0);

    return ret;
}

Channel * StreamClientChannel_New()
{
    Channel * thiz = NULL;

    do
    {
        thiz = SocketChannel_New();
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(StreamClientChannel_Construct(thiz)))
        {
            StreamClientChannel_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet StreamClientChannel_Initialize(Channel *thiz, ChannelInitializer initializer, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(initializer, TINY_RET_E_ARG_NULL);

    ((StreamClientChannelContext *)thiz->ctx) ->initializer = initializer ;
    ((StreamClientChannelContext *)thiz->ctx) ->initializerContext = ctx;

    return TINY_RET_OK;
}

/*
static void StreamClientChannel_InitializeWithRemoteInfo(Channel *thiz, const char *ip, uint16_t port)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(ip);

    tiny_snprintf(thiz->id, CHANNEL_ID_LEN, "%d::%s::%d", thiz->fd, ip, port);

    strncpy(thiz->remote.socket.ip, ip, IP_LEN);
    thiz->remote.socket.port = port;

    if (thiz->ctx != NULL)
    {
    	((StreamClientChannelContext *)thiz->ctx) ->initializer(thiz, ((StreamClientChannelContext *)thiz->ctx) ->initializerContext);
    }

    thiz->onActive(thiz);
}
*/

static void StreamClientChannel_SetRemoteInfo(Channel *thiz, const char* ip, uint16_t port)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(ip);

    tiny_snprintf(thiz->id, CHANNEL_ID_LEN, "%d::%s::%d", thiz->fd, ip, port);

    strncpy(thiz->remote.socket.ip, ip, IP_LEN);
    thiz->remote.socket.port = port;
}

TinyRet StreamClientChannel_Connect(Channel *thiz, const char *ip, uint16_t port)
{
    TinyRet ret = TINY_RET_OK;

    LOG_D(TAG, "StreamClientChannel_Connect: %s:%d", ip, port);

    do
    {
        ret = SocketChannel_Open(thiz, TYPE_TCP_CONNECTION);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_Open failed");
            break;
        }

        ret = SocketChannel_SetBlock(thiz, false);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_SetBlock failed");
            break;
        }

    	StreamClientChannel_SetRemoteInfo(thiz, ip, port);

	ret = SocketChannel_Connect(thiz, ip, port);
	if (ret == TINY_RET_OK)
	{
	    thiz->onActive(thiz);
	}

    } while (0);

    return ret;
}

TinyRet StreamClientChannel_Close(Channel* thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_INVALID);
    Channel_Close( thiz);
    return TINY_RET_OK;
}

