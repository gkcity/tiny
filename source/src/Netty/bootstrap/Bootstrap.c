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

#define TAG     "Bootstrap"

TINY_LOR
static void _OnChannelRemoved(void * data, void *ctx);

TINY_LOR
static TinyRet PreSelect(Selector *selector, void *ctx);

TINY_LOR
static TinyRet PostSelect(Selector *selector, void *ctx);

TINY_LOR
static TinyRet OnSelectTimeout(Selector *selector, void *ctx);

TINY_LOR
TinyRet Bootstrap_Construct(Bootstrap *thiz, BootstrapLoopHook preloop, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(Bootstrap));

        thiz->loopTimeout = EVENT_LOOP_DEFAULT_TIMEOUT;
        thiz->preloop = preloop;
        thiz->preloopCtx = ctx;

        ret = Selector_Construct(&thiz->selector);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "Selector_Construct FAILED");
            break;
        }

        thiz->selector.onPreSelect = PreSelect;
        thiz->selector.onPostSelect = PostSelect;
        thiz->selector.onSelectTimeout = OnSelectTimeout;
        thiz->selector.ctx = thiz;

        ret = TinyList_Construct(&thiz->channels, _OnChannelRemoved, NULL);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyList_Construct FAILED");
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

TINY_LOR
TinyRet Bootstrap_RemoveChannel(Bootstrap *thiz, Channel *channel)
{
    TinyRet ret = TINY_RET_E_NOT_FOUND;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(channel, TINY_RET_E_ARG_NULL);

    for (uint32_t i = 0; i < thiz->channels.size; ++i)
    {
        Channel *c = (Channel *) TinyList_GetAt(&thiz->channels, i);
        if (c->fd == channel->fd)
        {
            ret = TinyList_RemoveAt(&thiz->channels, i);
            break;
        }
    }

    return ret;
}

TINY_LOR
TinyRet Bootstrap_Sync(Bootstrap *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "Bootstrap_Sync");

    return Selector_Loop(&thiz->selector);
}

TINY_LOR
void Bootstrap_Shutdown(Bootstrap *thiz)
{
    if (thiz->selector.running)
    {
        thiz->selector.running = false;
    }
}

TINY_LOR
static void _OnChannelRemoved(void * data, void *ctx)
{
    Channel *channel = (Channel *) data;
    channel->_onRemove(channel);
}

TINY_LOR
static TinyRet PreSelect(Selector *selector, void *ctx)
{
    Bootstrap *thiz = (Bootstrap *)ctx;

    if (thiz->preloop != NULL)
    {
        thiz->preloop(thiz, thiz->preloopCtx);
    }

    for (uint32_t i = 0; i < thiz->channels.size; ++i)
    {
        Channel *channel = (Channel *)TinyList_GetAt(&thiz->channels, i);
        Channel_PreLoop(channel);
    }

    //LOG_I(TAG, "PreSelect, channels: %d", thiz->channels.size);

    for (int i = (int)(thiz->channels.size - 1); i >= 0; --i)
    {
        Channel *channel = (Channel *)TinyList_GetAt(&thiz->channels, i);
        if (Channel_IsClosed(channel))
        {
            LOG_I(TAG, "remove channel[%d]: %s, fd: %d", i, channel->id, channel->fd);
            TinyList_RemoveAt(&thiz->channels, i);
        }
    }

    //LOG_I(TAG, "current channels: %d", thiz->channels.size);

    if (thiz->channels.size == 0)
    {
        return TINY_RET_E_NOT_FOUND;
    }

    thiz->timer.valid = false;

    for (uint32_t i = 0; i < thiz->channels.size; ++i)
    {
        Channel *channel = (Channel *) TinyList_GetAt(&thiz->channels, i);
        channel->_onRegister(channel, selector, &thiz->timer);
    }

    if (thiz->timer.valid)
    {
        if (thiz->loopTimeout < thiz->timer.timeout)
        {
            thiz->timer.valid = false;
            thiz->selector.us = thiz->loopTimeout;
        }
        else
        {
            thiz->selector.us = thiz->timer.timeout;
        }
    }
    else
    {
        thiz->selector.us = thiz->loopTimeout;
    }

    return TINY_RET_OK;
}

TINY_LOR
static TinyRet PostSelect(Selector *selector, void *ctx)
{
    Bootstrap *thiz = (Bootstrap *)ctx;

    //LOG_I(TAG, "PostSelect");

    for (uint32_t i = 0; i < thiz->channels.size; ++i)
    {
        Channel *channel = (Channel *) TinyList_GetAt(&thiz->channels, i);

        if (Channel_IsActive(channel))
        {
            //LOG_I(TAG, "Channel: [%s] IsActive", channel->id);

            if (RET_FAILED(channel->_onAccess(channel, selector)))
            {
                LOG_E(TAG, "channel: [%s] onAccess failed", channel->id);
                channel->_onInactive(channel);
                Channel_Close(channel);
                continue;
            }
        }
    }

    return TINY_RET_OK;
}

TINY_LOR
static TinyRet OnSelectTimeout(Selector *selector, void *ctx)
{
    Bootstrap *thiz = (Bootstrap *)ctx;

    LOG_I(TAG, "OnSelectTimeout: %s", thiz->timer.valid ? "TIMER": "EVENT LOOP TIMEOUT");

    if (thiz->timer.valid)
    {
        for (uint32_t i = 0; i < thiz->channels.size; ++i)
        {
            Channel *channel = (Channel *)TinyList_GetAt(&thiz->channels, i);
            channel->_onEventTriggered(channel, &thiz->timer);
        }
    }

    return TINY_RET_OK;
}