/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   Bootstrap.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_log.h>
#include <channel/SocketChannel.h>
#include "Bootstrap.h"
#include "LoopbackChannelHandler.h"

#define TAG     "Bootstrap"

TINY_LOR
static void _OnChannelRemoved(void * data, void *ctx);

TINY_LOR
static TinyRet _PreSelect(Selector *selector, void *ctx);

TINY_LOR
static TinyRet _PostSelect(Selector *selector, void *ctx);

TINY_LOR
static TinyRet _OnSelectTimeout(Selector *selector, void *ctx);

TINY_LOR
TinyRet Bootstrap_InitializeLoopbackChannel(Bootstrap *thiz)
{
    Channel * loopback = NULL;

    loopback = SocketChannel_New();
    if (loopback == NULL)
    {
        return TINY_RET_E_NEW;
    }

    SocketChannel_Open(loopback, TYPE_UDP);
    SocketChannel_Bind(loopback, 0, false);
    SocketChannel_SetBlock(loopback, false);
    SocketChannel_AddLast(loopback, LoopbackChannelHandler(&thiz->channels));

    thiz->loopbackPort = loopback->local.socket.port;

    return TinyList_AddTail(&thiz->channels, loopback);
}

TINY_LOR
TinyRet Bootstrap_Construct(Bootstrap *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        ret = Selector_Construct(&thiz->selector);
        if (RET_FAILED(ret))
        {
            break;
        }

        thiz->selector.onPreSelect = _PreSelect;
        thiz->selector.onPostSelect = _PostSelect;
        thiz->selector.onSelectTimeout = _OnSelectTimeout;
        thiz->selector.ctx = thiz;

        ret = TinyList_Construct(&thiz->channels);
        if (RET_FAILED(ret))
        {
            break;
        }
        TinyList_SetDeleteListener(&thiz->channels, _OnChannelRemoved, NULL);

        ret = Bootstrap_InitializeLoopbackChannel(thiz);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

TINY_LOR
void Bootstrap_Dispose(Bootstrap *thiz)
{
    RETURN_IF_FAIL(thiz);

    LOG_D(TAG, "Bootstrap_Dispose");

    Selector_Dispose(&thiz->selector);
    TinyList_Dispose(&thiz->channels);
}

TINY_LOR
TinyRet Bootstrap_AddChannel(Bootstrap *thiz, Channel *channel)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(channel, TINY_RET_E_ARG_NULL);

    return TinyList_AddTail(&thiz->channels, channel);
}

//TINY_LOR
//TinyRet Bootstrap_RemoveChannel(Bootstrap *thiz, Channel *channel)
//{
//    TinyRet ret = TINY_RET_E_NOT_FOUND;
//
//    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
//    RETURN_VAL_IF_FAIL(channel, TINY_RET_E_ARG_NULL);
//
//    for (uint32_t i = 0; i < thiz->channels.size; ++i)
//    {
//        Channel *c = (Channel *) TinyList_GetAt(&thiz->channels, i);
//        if (c->fd == channel->fd)
//        {
//            ret = TinyList_RemoveAt(&thiz->channels, i);
//            break;
//        }
//    }
//
//    return ret;
//}

TINY_LOR
TinyRet Bootstrap_Sync(Bootstrap *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "Bootstrap_Sync");

    return Selector_Loop(&thiz->selector);
}

TINY_LOR
TinyRet Bootstrap_Shutdown(Bootstrap *thiz)
{
    int ret = 0;
    uint32_t length = (uint32_t)strlen(BOOTSTRAP_SHUTDOWN);

    struct sockaddr_in to;
    socklen_t to_len = sizeof(to);

    Channel *channel = SocketChannel_New();
    SocketChannel_Open(channel, TYPE_UDP);
    SocketChannel_Bind(channel, 0, false);

    memset(&to, 0, sizeof(to));
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = inet_addr("127.0.0.1");
    to.sin_port = htons(thiz->loopbackPort);
    ret = (int)tiny_sendto(channel->fd, BOOTSTRAP_SHUTDOWN, length, 0, (struct sockaddr *)&to, (socklen_t)to_len);
    LOG_D(TAG, "sendto: 127.0.0.0:%d %d", thiz->loopbackPort, ret);

    SocketChannel_Delete(channel);

    return (ret == length) ? TINY_RET_OK : TINY_RET_E_INTERNAL;
}

TINY_LOR
static void _OnChannelRemoved(void * data, void *ctx)
{
    Channel *channel = (Channel *) data;
    channel->onRemove(channel);
}

TINY_LOR
static TinyRet _PreSelect(Selector *selector, void *ctx)
{
    Bootstrap *thiz = (Bootstrap *)ctx;

    LOG_D(TAG, "_PreSelect, channels: %d", thiz->channels.size);

    for (int i = (thiz->channels.size - 1); i >= 0; --i)
    {
        Channel *channel = (Channel *)TinyList_GetAt(&thiz->channels, i);

        LOG_D(TAG, "channel[%d]: %s, fd: %d", i, channel->id, channel->fd);

        if (Channel_IsClosed(channel))
        {
            TinyList_RemoveAt(&thiz->channels, i);
        }
    }

    if (thiz->channels.size == 0)
    {
        LOG_E(TAG, "Channels is empty");
        return TINY_RET_E_NOT_FOUND;
    }

    thiz->timer.valid = false;

    for (uint32_t i = 0; i < thiz->channels.size; ++i)
    {
        Channel *channel = (Channel *) TinyList_GetAt(&thiz->channels, i);
        channel->onRegister(channel, selector);

        if (channel->getTimeout != NULL)
        {
            if (RET_SUCCEEDED(channel->getTimeout(channel, &thiz->timer, NULL)))
            {
                thiz->timer.fd = channel->fd;
            }
        }
    }

    thiz->selector.us = (thiz->timer.valid) ? thiz->timer.timeout : 0;

    return TINY_RET_OK;
}

TINY_LOR
static TinyRet _PostSelect(Selector *selector, void *ctx)
{
    Bootstrap *thiz = (Bootstrap *)ctx;

    LOG_D(TAG, "_PostSelect");

    for (uint32_t i = 0; i < thiz->channels.size; ++i)
    {
        Channel *channel = (Channel *) TinyList_GetAt(&thiz->channels, i);
        if (Channel_IsActive(channel))
        {
            if (RET_FAILED(channel->onReadWrite(channel, selector)))
            {
                LOG_D(TAG, "channel [%d] read or write failed", channel->fd);
                channel->onInactive(channel);
                Channel_Close(channel);
                continue;
            }
        }
    }

    return TINY_RET_OK;
}

TINY_LOR
static TinyRet _OnSelectTimeout(Selector *selector, void *ctx)
{
    Bootstrap *thiz = (Bootstrap *)ctx;

    LOG_D(TAG, "_OnSelectTimeout");

    for (uint32_t i = 0; i < thiz->channels.size; ++i)
    {
        Channel *channel = (Channel *)TinyList_GetAt(&thiz->channels, i);
        if (channel->fd == thiz->timer.fd)
        {
            channel->onEventTriggered(channel, &thiz->timer);
        }
    }

    return TINY_RET_OK;
}