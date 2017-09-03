/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   LoopbackChannelHandler.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_log.h>
#include <tiny_malloc.h>
#include "LoopbackChannelHandler.h"

#define TAG "LoopbackChannelHandler"

TINY_LOR
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    LOG_D(TAG, "_channelRead: %d bytes from %s", len, channel->id);

    if (len == strlen(BOOTSTRAP_SHUTDOWN))
    {
        if (STR_EQUAL(BOOTSTRAP_SHUTDOWN, data))
        {
            TinyList *list = thiz->data;

            for (uint32_t i = 0; i < list->size; ++i)
            {
                Channel *c = (Channel *) TinyList_GetAt(list, i);
                c->_close(c);
            }
        }
    }

    return true;
}

TINY_LOR
static TinyRet LoopbackChannelHandler_Dispose(ChannelHandler *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(ChannelHandler));

    return TINY_RET_OK;
}

TINY_LOR
static void LoopbackChannelHandler_Delete(ChannelHandler *thiz)
{
    LOG_D(TAG, "LoopbackChannelHandler_Delete");

    LoopbackChannelHandler_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
static TinyRet LoopbackChannelHandler_Construct(ChannelHandler *thiz, void *ctx)
{
    LOG_D(TAG, "LoopbackChannelHandler_Construct");

    memset(thiz, 0, sizeof(ChannelHandler));

    strncpy(thiz->name, LoopbackChannelHandler_Name, CHANNEL_HANDLER_NAME_LEN);
    thiz->onRemove = LoopbackChannelHandler_Delete;
    thiz->inType = DATA_RAW;
    thiz->outType = DATA_RAW;
    thiz->channelRead = _channelRead;
    thiz->data = ctx;

    return TINY_RET_OK;
}

TINY_LOR
ChannelHandler * LoopbackChannelHandler(void *ctx)
{
    ChannelHandler *thiz = NULL;

    do
    {
        thiz = (ChannelHandler *)tiny_malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(LoopbackChannelHandler_Construct(thiz, ctx)))
        {
            LoopbackChannelHandler_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}