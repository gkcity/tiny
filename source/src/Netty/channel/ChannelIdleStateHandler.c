/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ChannelIdleStateHandler.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "ChannelIdleStateHandler.h"

#define TAG     "ChannelIdleStateHandler"

TINY_LOR
static TinyRet ChannelIdleStateHandler_Construct(ChannelHandler *thiz, uint32_t readerIdle, uint32_t writerIdle, uint32_t allIdle);

TINY_LOR
static TinyRet ChannelIdleStateHandler_Dispose(ChannelHandler *thiz);

TINY_LOR
static void SocketChannelIdleStateHandler_Delete(ChannelHandler *thiz);

TINY_LOR
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len);

TINY_LOR
static bool _channelWrite(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len);

TINY_LOR
static int64_t _channelGetNextTimeout(Channel *channel, void *ctx);

TINY_LOR
ChannelHandler * ChannelIdleStateHandler(uint32_t readerIdle, uint32_t writerIdle, uint32_t allIdle)
{
    ChannelHandler *thiz = NULL;

    do
    {
        thiz = (ChannelHandler *)tiny_malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(ChannelIdleStateHandler_Construct(thiz, readerIdle, writerIdle, allIdle)))
        {
            SocketChannelIdleStateHandler_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
static void SocketChannelIdleStateHandler_Delete(ChannelHandler *thiz)
{
    ChannelIdleStateHandler_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
static TinyRet ChannelIdleStateHandler_Construct(ChannelHandler *thiz, uint32_t readerIdle, uint32_t writerIdle, uint32_t allIdle)
{
    memset(thiz, 0, sizeof(ChannelHandler));

    strncpy(thiz->name, ChannelIdleStateHandler_Name, CHANNEL_HANDLER_NAME_LEN);
	thiz->onRemove = SocketChannelIdleStateHandler_Delete;
    thiz->inType = DATA_RAW;
    thiz->outType = DATA_RAW;
    thiz->channelRead = _channelRead;
    thiz->channelWrite = _channelWrite;
    thiz->getNextTimeout = _channelGetNextTimeout;

    ChannelIdles_Initialize(&thiz->idles, readerIdle, writerIdle, allIdle);

    return TINY_RET_OK;
}

TINY_LOR
static TinyRet ChannelIdleStateHandler_Dispose(ChannelHandler *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(ChannelHandler));

    return TINY_RET_OK;
}

TINY_LOR
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    LOG_D(TAG, "_channelRead: %d bytes from %s", len, channel->id);

    ChannelIdles_OnRead(&thiz->idles);

    return false;
}

TINY_LOR
static bool _channelWrite(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    LOG_D(TAG, "_channelWrite: %d bytes to %s", len, channel->id);

    ChannelIdles_OnWrite(&thiz->idles);

	return false;
}

TINY_LOR
static int64_t _channelGetNextTimeout(Channel *channel, void *ctx)
{
    LOG_D(TAG, "_channelGetNextTimeout, channelId: %s", channel->id);

    return ChannelIdles_GetNextTimeout(&((ChannelHandler *)ctx)->idles);
}