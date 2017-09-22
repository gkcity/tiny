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
#include <tiny_socket.h>
#include "StreamClientChannel.h"
#include "StreamClientChannelContext.h"
#include "channel/SocketChannel.h"

#define TAG     "StreamClientChannel"

TINY_LOR
static TinyRet StreamClientChannel_GetConnectingTimeout(Channel *thiz, ChannelTimer *timer, void *ctx)
{
    timer->timeout = ((StreamClientChannelContext *) thiz->ctx)->connectingTimeout * 1000000;
    timer->type = CHANNEL_TIMER_ALL;
    return TINY_RET_OK;
}

TINY_LOR
void StreamClientChannel_Dispose(Channel *thiz)
{
    if (thiz->ctx != NULL)
    {
        StreamClientChannelContext_Delete((StreamClientChannelContext *) thiz->ctx);
        thiz->ctx = NULL;
    }

    if (StreamClientChannel_IsConnected(thiz))
    {
        StreamClientChannel_Close(thiz);
    }

    SocketChannel_Dispose(thiz);
}

TINY_LOR
void StreamClientChannel_Delete(Channel *thiz)
{
    StreamClientChannel_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
static void StreamClientChannel_OnRegister(Channel *thiz, Selector *selector, ChannelTimer *timer)
{
    if (Channel_IsActive(thiz))
    {
        Selector_Register(selector, thiz->fd, SELECTOR_OP_ALL);

        if (thiz->_getTimeout != NULL)
        {
            if (RET_SUCCEEDED(thiz->_getTimeout(thiz, timer, NULL)))
            {
                timer->fd = thiz->fd;
            }
        }
    }
}

TINY_LOR
static void StreamClientChannel_OnRemove(Channel *thiz)
{
    LOG_D(TAG, "StreamClientChannel_OnRemove");

    StreamClientChannel_Delete(thiz);
}

TINY_LOR
static TinyRet StreamClientChannel_OnReadWrite(Channel *thiz, Selector *selector)
{
    LOG_D(TAG, "StreamClientChannel_OnReadWrite");

    // [1] socket is writeable if connection established.
    // [2] socket is readable and writeable if connection occur error.
    if (Selector_IsReadable(selector, thiz->fd) || Selector_IsWriteable(selector, thiz->fd))
    {
        if (tiny_socket_has_error(thiz->fd))
        {
            LOG_E(TAG, "tiny_socket_has_error!");
            return TINY_RET_E_SOCKET_DISCONNECTED;
        }

        thiz->_onActive(thiz);
    }

    return TINY_RET_OK;
}

TINY_LOR
static void StreamClientChannel_OnActive(Channel *thiz)
{
    RETURN_IF_FAIL(thiz);

    LOG_D(TAG, "StreamClientChannel_OnActive");

    if (thiz->ctx != NULL)
    {
        ((StreamClientChannelContext *) thiz->ctx)->initializer(thiz, ((StreamClientChannelContext *) thiz->ctx)->initializerContext);
    }

    thiz->_onActive = SocketChannel_OnActive;
    thiz->_onReadWrite = SocketChannel_OnReadWrite;
    thiz->_onRegister = SocketChannel_OnRegister;
    thiz->_getTimeout = SocketChannel_GetTimeout;
    thiz->_onActive(thiz);
}

TINY_LOR
TinyRet StreamClientChannel_Construct(Channel *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        thiz->_onRegister = StreamClientChannel_OnRegister;
        thiz->_onRemove = StreamClientChannel_OnRemove;
        thiz->_onActive = StreamClientChannel_OnActive;
        thiz->_onReadWrite = StreamClientChannel_OnReadWrite;

        thiz->_getTimeout = StreamClientChannel_GetConnectingTimeout;

        thiz->ctx = StreamClientChannelContext_New();
        if (thiz->ctx == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }
    } while (0);

    return ret;
}

TINY_LOR
Channel *StreamClientChannel_New()
{
    Channel *thiz = NULL;

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

TINY_LOR
TinyRet StreamClientChannel_Initialize(Channel *thiz, ChannelInitializer initializer, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(initializer, TINY_RET_E_ARG_NULL);

    ((StreamClientChannelContext *) thiz->ctx)->initializer = initializer;
    ((StreamClientChannelContext *) thiz->ctx)->initializerContext = ctx;

    return TINY_RET_OK;
}

TINY_LOR
TinyRet StreamClientChannel_Connect(Channel *thiz, const char *ip, uint16_t port, uint32_t timeout)
{
    TinyRet ret = TINY_RET_OK;

    LOG_I(TAG, "StreamClientChannel_Connect: %s:%d", ip, port);

    do
    {
        ret = SocketChannel_Open(thiz, TYPE_TCP_CONNECTION);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_Open failed: %d", TINY_RET_CODE(ret));
            break;
        }

        ret = SocketChannel_SetBlock(thiz, false);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_SetBlock failed: %d", TINY_RET_CODE(ret));
            break;
        }

        ((StreamClientChannelContext *) thiz->ctx)->connectingTimeout = timeout;

        SocketChannel_SetRemoteInfo(thiz, ip, port);

        ret = tiny_async_connect(thiz->fd, ip, port);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "tiny_async_connect failed: %d", TINY_RET_CODE(ret));
            break;
        }

        if (TINY_RET_CODE(ret) == CODE_PENDING)
        {
            LOG_D(TAG, "connecting is pending");
            break;
        }

        thiz->_onActive(thiz);
    } while (0);

    return ret;
}

TINY_LOR
TinyRet StreamClientChannel_Close(Channel *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_INVALID);
    Channel_Close(thiz);
    return TINY_RET_OK;
}

TINY_LOR
bool StreamClientChannel_IsConnected(Channel *thiz)
{
    return (thiz->fd >= 0);
}