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

static void StreamServerChannel_Dispose(Channel *thiz)
{
    if (thiz->ctx != NULL)
    {
        StreamServerChannelContext_Delete((StreamServerChannelContext *)thiz->ctx);
        thiz->ctx = NULL;
    }
}

static void StreamServerChannel_Delete(Channel *thiz)
{
    StreamServerChannel_Dispose(thiz);
    tiny_free(thiz);
}

static void StreamServerChannel_OnRegister(Channel *thiz, Selector *selector)
{
    if (Channel_IsActive(thiz))
    {
        StreamServerChannelContext *ctx = (StreamServerChannelContext *)thiz->ctx;
        Selector_Register(selector, thiz->fd, SELECTOR_OP_READ);
        LOG_D(TAG, "StreamServerChannel_OnRegister: %d", thiz->fd);

        for (int i = 0; i < TinyList_GetCount(&ctx->channels); ++i)
        {
            Channel *c = (Channel *) TinyList_GetAt(&ctx->channels, i);
            Selector_Register(selector, c->fd, SELECTOR_OP_READ);
            LOG_D(TAG, "StreamServerChannel_OnRegister: %d", c->fd);
        }
    }
}

static void StreamServerChannel_OnRemove(Channel *thiz)
{
    LOG_D(TAG, "StreamServerChannel_OnRemove");

    StreamServerChannel_Delete(thiz);
}

static TinyRet StreamServerChannel_OnRead(Channel *thiz, Selector *selector)
{
    StreamServerChannelContext *ctx = (StreamServerChannelContext *)thiz->ctx;

    if (Selector_IsReadable(selector, thiz->fd))
    {
        Channel *newChannel = NULL;
        int fd = 0;
        struct sockaddr_in addr;
        socklen_t len = (socklen_t) sizeof(addr);
        const char *ip = NULL;
        uint16_t port = 0;

        LOG_D(TAG, "StreamServerChannel_OnRead");

        memset(&addr, 0, sizeof(addr));
        fd = accept(thiz->fd, (struct sockaddr *)&addr, &len);
        if (fd <= 0)
        {
            return TINY_RET_E_INTERNAL;
        }

        ip = inet_ntoa(addr.sin_addr);
        port = ntohs(addr.sin_port);

        LOG_D(TAG, "accept a new connection: %s:%d, fd:%d", ip, port, fd);

        newChannel = SocketChannel_New();
        SocketChannel_InitializeWithRemoteInfo(newChannel, ip, fd, port, ctx->initializer, ctx->initializerContext);

        TinyList_AddTail(&ctx->channels, newChannel);

        return TINY_RET_OK;
    }

    for (int i = 0; i < TinyList_GetCount(&ctx->channels); ++i)
    {
        Channel *channel = (Channel *) TinyList_GetAt(&ctx->channels, i);
        if (Channel_IsActive(channel))
        {
            if (RET_FAILED(channel->onRead(channel, selector)))
            {
                LOG_D(TAG, "close connection: %s:%d, fd:%d", channel->local.socket.ip, channel->local.socket.port, channel->fd);
                channel->onInactive(channel);
                Channel_Close(channel);
            }
        }
    }

    for (int i = 0; i < TinyList_GetCount(&ctx->channels); ++i)
    {
        Channel *channel = (Channel *)TinyList_GetAt(&ctx->channels, i);
        if (Channel_IsClosed(channel))
        {
            TinyList_RemoveAt(&ctx->channels, i);
            break;
        }
    }

    return TINY_RET_OK;
}

static void StreamServerChannel_OnInactive(Channel *thiz)
{
    StreamServerChannelContext *ctx = (StreamServerChannelContext *)thiz->ctx;

    LOG_D(TAG, "StreamServerChannel_OnInactive");

    for (int i = 0; i < TinyList_GetCount(&ctx->channels); ++i)
    {
        Channel *channel = (Channel *)TinyList_GetAt(&ctx->channels, i);
        channel->onInactive(channel);
        Channel_Close(channel);
    }
}

static TinyRet StreamServerChannel_Construct(Channel *thiz, int maxConnections)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(Channel));

        thiz->onRegister = StreamServerChannel_OnRegister;
        thiz->onRemove = StreamServerChannel_OnRemove;
        thiz->onInactive = StreamServerChannel_OnInactive;
        thiz->onRead = StreamServerChannel_OnRead;

        thiz->ctx = StreamServerChannelContext_New();
        if (thiz->ctx == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }

        ((StreamServerChannelContext *)thiz->ctx)->maxConnections = maxConnections;
    } while (0);

    return ret;
}

Channel * StreamServerChannel_New(int maxConnections)
{
    Channel * thiz = NULL;

    do
    {
        thiz = (Channel *)tiny_malloc(sizeof(Channel));
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

TinyRet StreamServerChannel_Initialize(Channel *thiz, ChannelInitializer initializer, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(initializer, TINY_RET_E_ARG_NULL);

    ((StreamServerChannelContext *)thiz->ctx) ->initializer = initializer ;
    ((StreamServerChannelContext *)thiz->ctx) ->initializerContext = ctx;

    return TINY_RET_OK;
}

TinyRet StreamServerChannel_Bind(Channel *thiz, uint16_t port)
{
    TinyRet ret = TINY_RET_OK;

    LOG_D(TAG, "StreamServerChannel_Bind: %d", port);

    do
    {
        ret = SocketChannel_Open(thiz, TYPE_TCP_SERVER);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_Open failed");
            break;
        }

        ret = SocketChannel_Bind(thiz, port);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_Bind failed");
            break;
        }

        ret = SocketChannel_SetBlock(thiz, false);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_SetBlock failed");
            break;
        }

        ret = SocketChannel_Listen(thiz, ((StreamServerChannelContext *)thiz->ctx)->maxConnections);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_Listen failed");
            break;
        }
    } while (0);

    return ret;
}