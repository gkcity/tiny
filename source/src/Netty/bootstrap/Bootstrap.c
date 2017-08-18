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
#include <tiny_malloc.h>
#include "Bootstrap.h"

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
TinyRet Bootstrap_Construct(Bootstrap *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        thiz->timerConfiguration.type = TIMER_TYPE_NONE;
        thiz->timerConfiguration.index = 0;

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

    LOG_MEM(TAG, "Bootstrap_Sync");

    for (uint32_t i = 0; i < thiz->channels.size; ++i)
    {
        Channel *channel = (Channel *) TinyList_GetAt(&thiz->channels, i);

        if (channel == NULL)
        {
            printf("Channel is NULL\n");
            continue;
        }

        if (channel->onActive != NULL)
        {
            channel->onActive(channel);
        }
    }

    return Selector_Loop(&thiz->selector);
}

TINY_LOR
TinyRet Bootstrap_Shutdown(Bootstrap *thiz)
{
    LOG_D(TAG, "Bootstrap_Shutdown");

    for (uint32_t i = 0; i < thiz->channels.size; ++i)
    {
        Channel *channel = (Channel *) TinyList_GetAt(&thiz->channels, i);
        channel->onInactive(channel);
        Channel_Close(channel);
    }

    return TINY_RET_OK;
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
    TinyRet ret = TINY_RET_OK;
    Bootstrap *thiz = (Bootstrap *)ctx;

    LOG_D(TAG, "_PreSelect");

    thiz->timerConfiguration.type = TIMER_TYPE_NONE;
    thiz->timerConfiguration.us = (int64_t) 3600000000;
    thiz->timerConfiguration.index = 0;

    for (uint32_t i = 0; i < thiz->channels.size; ++i)
    {
        Channel *channel = (Channel *) TinyList_GetAt(&thiz->channels, i);
        channel->onRegister(channel, selector);

        if (channel->getNextTimeout != NULL)
        {
            int64_t idle = channel->getNextTimeout(channel, NULL);
            if (idle > 0 && idle < thiz->timerConfiguration.us)
            {
                thiz->timerConfiguration.type = TIMER_TYPE_CHANNEL_TIMER;
                thiz->timerConfiguration.index = i;
                thiz->timerConfiguration.us = idle;
            }
        }
    }

    thiz->selector.us = thiz->timerConfiguration.type == TIMER_TYPE_NONE ? 0 : thiz->timerConfiguration.us;

    return ret;
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
            if (RET_FAILED(channel->onRead(channel, selector)))
            {
                LOG_D(TAG, "channel [%d] read failed", channel->fd);
                channel->onInactive(channel);
                Channel_Close(channel);
                continue;
            }

            if (RET_FAILED(channel->onWrite(channel, selector)))
            {
                LOG_D(TAG, "channel [%s] write failed", channel->id);
                channel->onInactive(channel);
                Channel_Close(channel);
            }
        }
    }

    for (uint32_t i = 0; i < thiz->channels.size; ++i)
    {
        Channel *channel = (Channel *)TinyList_GetAt(&thiz->channels, i);
        if (Channel_IsClosed(channel))
        {
            TinyList_RemoveAt(&thiz->channels, i);
            break;
        }
    }

    return TINY_RET_OK;
}

TINY_LOR
static TinyRet _OnSelectTimeout(Selector *selector, void *ctx)
{
    Bootstrap *thiz = (Bootstrap *)ctx;
    Channel *channel = NULL;

    LOG_D(TAG, "_OnSelectTimeout");

    switch (thiz->timerConfiguration.type)
    {
        case TIMER_TYPE_CHANNEL_TIMER:
            LOG_D(TAG, "TIMER TYPE: channel timer");
            channel = (Channel *)TinyList_GetAt(&thiz->channels, thiz->timerConfiguration.index);
            channel->onEventTriggered(channel, NULL);
            break;

        default:
            LOG_D(TAG, "TIMER TYPE: unknown: %d", thiz->timerConfiguration.type);
            break;
    }

    return TINY_RET_OK;
}