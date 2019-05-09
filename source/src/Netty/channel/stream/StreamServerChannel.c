/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   StreamServerChannel.c
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
#include "StreamServerChannel.h"
#include "StreamServerChannelContext.h"
#include "channel/SocketChannel.h"

#define TAG     "StreamServerChannel"

TINY_LOR
static void StreamServerChannel_Dispose(Channel *thiz)
{
    if (thiz->context != NULL)
    {
        StreamServerChannelContext_Delete((StreamServerChannelContext *)thiz->context);
        thiz->context = NULL;
    }

    SocketChannel_Dispose(thiz);
}

TINY_LOR
static void StreamServerChannel_Delete(Channel *thiz)
{
    StreamServerChannel_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
static void StreamServerChannel_OnRegister(Channel *thiz, Selector *selector, ChannelTimer *timer)
{
    if (Channel_IsActive(thiz))
    {
        StreamServerChannelContext *ctx = (StreamServerChannelContext *)thiz->context;
        Selector_Register(selector, thiz->fd, SELECTOR_OP_READ);

        // remove closed channel
        for (int i = (ctx->channels.size - 1); i >= 0; --i)
        {
            Channel* child = (Channel *)TinyList_GetAt(&ctx->channels, i);
            if (Channel_IsClosed(child))
            {
                LOG_D(TAG, "remove channel[%d]: %s, fd: %d", i, child->id, child->fd);
                TinyList_RemoveAt(&ctx->channels, i);
            }
        }

        for (uint32_t i = 0; i < ctx->channels.size; ++i)
        {
            Channel *child = (Channel *) TinyList_GetAt(&ctx->channels, i);
            SocketChannel_OnRegister(child, selector, timer);
        }
    }
}

TINY_LOR
static void StreamServerChannel_OnRemove(Channel *thiz)
{
    LOG_D(TAG, "StreamServerChannel_OnRemove: %s", thiz->id);

    StreamServerChannel_Delete(thiz);
}

TINY_LOR
static TinyRet StreamServerChannel_OnAccept(Channel *thiz, Selector *selector)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        if (! Selector_IsReadable(selector, thiz->fd))
        {
            break;
        }

        LOG_I(TAG, "StreamServerChannel_OnAccept: %s", thiz->id);

        StreamServerChannelContext *ctx = (StreamServerChannelContext *)thiz->context;
        Channel *newChannel = NULL;
        int fd = 0;
        struct sockaddr_in addr;
        socklen_t len = (socklen_t) sizeof(struct sockaddr_in);
        char ip[TINY_IP_LEN];
        uint16_t port = 0;

        memset(&addr, 0, sizeof(struct sockaddr_in));

        fd = (int) tiny_accept(thiz->fd, (struct sockaddr *)&addr, &len);
        if (fd < 0)
        {
            LOG_E(TAG, "tiny_accept: %d", fd);
            ret = tiny_socket_has_error(thiz->fd) ? TINY_RET_E_SOCKET_ASCEPT : TINY_RET_OK;
            break;
        }

        memset(ip, 0, TINY_IP_LEN);
        inet_ntop(AF_INET, &addr.sin_addr, ip, TINY_IP_LEN);
        port = ntohs(addr.sin_port);

        LOG_I(TAG, "Accept a new connection: %d#%s:%d", fd, ip, port);

//        newChannel = SocketChannel_NewCustomBufferSize(thiz->inBufferSize, thiz->outBufferSize);
        newChannel = SocketChannel_NewCustomBufferSize(thiz->inBufferSize, 0);
        if (newChannel == NULL)
        {
            LOG_E(TAG, "SocketChannel_New NULL");
            break;
        }

        newChannel->fd = fd;
        SocketChannel_SetRemoteInfo(newChannel, ip, port);
        SocketChannel_Initialize(newChannel, ctx->initializer, ctx->initializerContext);

        TinyList_AddTail(&ctx->channels, newChannel);
    } while (false);

    return ret;
}

TINY_LOR
static void StreamServerChannel_OnConnectionAccess(Channel *thiz, Selector *selector)
{
    StreamServerChannelContext *ctx = (StreamServerChannelContext *)thiz->context;

    for (uint32_t i = 0; i < ctx->channels.size; ++i)
    {
        Channel *channel = (Channel *) TinyList_GetAt(&ctx->channels, i);

        if (Channel_IsActive(channel))
        {
            if (RET_FAILED(channel->_onAccess(channel, selector)))
            {
                LOG_D(TAG, "close connection: %d#%s#%d", channel->fd, channel->local.socket.ip, channel->local.socket.port);
                channel->_onInactive(channel);
                Channel_Close(channel);
            }
        }
    }
}

TINY_LOR
static void StreamServerChannel_RemoveClosedConnection(Channel *thiz)
{
    StreamServerChannelContext *ctx = (StreamServerChannelContext *)thiz->context;

    for (uint32_t i = 0; i < ctx->channels.size; ++i)
    {
        Channel *channel = (Channel *)TinyList_GetAt(&ctx->channels, i);
        if (Channel_IsClosed(channel))
        {
            TinyList_RemoveAt(&ctx->channels, i);
            break;
        }
    }
}

TINY_LOR
static TinyRet StreamServerChannel_OnAccess(Channel *thiz, Selector *selector)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(selector, TINY_RET_E_ARG_NULL);

//    LOG_I(TAG, "StreamServerChannel_OnAccess: %s", thiz->id);

    ret = StreamServerChannel_OnAccept(thiz, selector);
    if (RET_SUCCEEDED(ret))
    {
        StreamServerChannel_OnConnectionAccess(thiz, selector);
        StreamServerChannel_RemoveClosedConnection(thiz);
    }

    return ret;
}

TINY_LOR
static void StreamServerChannel_OnInactive(Channel *thiz)
{
    StreamServerChannelContext *ctx = (StreamServerChannelContext *)thiz->context;

    LOG_D(TAG, "StreamServerChannel_OnInactive");

    for (uint32_t i = 0; i < ctx->channels.size; ++i)
    {
        Channel *channel = (Channel *)TinyList_GetAt(&ctx->channels, i);
        if (Channel_IsActive(channel)) 
        {
            channel->_onInactive(channel);
        }
    }
}

TINY_LOR
static void StreamServerChannel_OnEventTriggered(Channel *thiz, ChannelTimer *timer)
{
    StreamServerChannelContext *ctx = (StreamServerChannelContext *)thiz->context;

    RETURN_IF_FAIL(thiz);

//    LOG_D(TAG, "StreamServerChannel_OnEventTriggered");

    for (uint32_t i = 0; i < ctx->channels.size; ++i)
    {
        Channel *channel = (Channel *)TinyList_GetAt(&ctx->channels, i);
        channel->_onEventTriggered(channel, timer);
    }
}

TINY_LOR
static void StreamServerChannel_Close(Channel *thiz)
{
    StreamServerChannelContext *ctx = (StreamServerChannelContext *)thiz->context;

    LOG_D(TAG, "StreamServerChannel_Close");

    for (uint32_t i = 0; i < ctx->channels.size; ++i)
    {
        Channel *channel = (Channel *)TinyList_GetAt(&ctx->channels, i);
        if (Channel_IsActive(channel))
        {
            channel->_onInactive(channel);
            channel->_close(channel);
        }
    }

    LOG_D(TAG, "StreamServerChannel_Close: %s", thiz->id);

    Channel_Close(thiz);
}

TINY_LOR
static TinyRet StreamServerChannel_Construct(Channel *thiz, int maxConnections)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "StreamServerChannel_Construct, maxConnections: %d", maxConnections);

    do
    {
        thiz->_onRegister = StreamServerChannel_OnRegister;
        thiz->_onRemove = StreamServerChannel_OnRemove;
        thiz->_onInactive = StreamServerChannel_OnInactive;
        thiz->_onAccess = StreamServerChannel_OnAccess;
        thiz->_onEventTriggered = StreamServerChannel_OnEventTriggered;
        thiz->_close = StreamServerChannel_Close;

        thiz->context = StreamServerChannelContext_New();
        if (thiz->context == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }

        ((StreamServerChannelContext *)thiz->context)->maxConnections = maxConnections;
    } while (0);

    return ret;
}

TINY_LOR
Channel * StreamServerChannel_New(int maxConnections)
{
    Channel * thiz = NULL;

    do
    {
        thiz = SocketChannel_New();
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(StreamServerChannel_Construct(thiz, maxConnections)))
        {
            StreamServerChannel_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void StreamServerChannel_Initialize(Channel *thiz, ChannelInitializer initializer, void *ctx)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(initializer);

    ((StreamServerChannelContext *)thiz->context) ->initializer = initializer;
    ((StreamServerChannelContext *)thiz->context) ->initializerContext = ctx;
}