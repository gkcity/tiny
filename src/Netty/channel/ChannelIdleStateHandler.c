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

static TinyRet ChannelIdleStateHandler_Construct(ChannelHandler *thiz, uint32_t readerIdle, uint32_t writerIdle, uint32_t allIdle);
static TinyRet ChannelIdleStateHandler_Dispose(ChannelHandler *thiz);
static void SocketChannelIdleStateHandler_Delete(ChannelHandler *thiz);
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, void *data, uint32_t len);
static bool _channelWrite(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len);
static int64_t _channelNextTimeout(Channel *channel, void *ctx);

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

static void SocketChannelIdleStateHandler_Delete(ChannelHandler *thiz)
{
    ChannelIdleStateHandler_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet ChannelIdleStateHandler_Construct(ChannelHandler *thiz, uint32_t readerIdle, uint32_t writerIdle, uint32_t allIdle)
{
    memset(thiz, 0, sizeof(ChannelHandler));

    strncpy(thiz->name, ChannelIdleStateHandler_Name, CHANNEL_HANDLER_NAME_LEN);
	thiz->onRemove = SocketChannelIdleStateHandler_Delete;
    thiz->inType = DATA_RAW;
    thiz->outType = DATA_RAW;
    thiz->channelRead = _channelRead;
    thiz->channelWrite = _channelWrite;
    thiz->nextTimeout = _channelNextTimeout;
    thiz->idle.readerIdleTimeSeconds = readerIdle;
    thiz->idle.writerIdleTimeSeconds = writerIdle;
    thiz->idle.allIdleTimeSeconds = allIdle;

    return TINY_RET_OK;
}

static TinyRet ChannelIdleStateHandler_Dispose(ChannelHandler *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(ChannelHandler));

    return TINY_RET_OK;
}

static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, void *data, uint32_t len)
{
    LOG_D(TAG, "_channelRead: %d bytes from %s", len, channel->id);

    // TODO: 计算读超时时间

    // 没有处理，返回false
    return false;
}

static bool _channelWrite(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    LOG_D(TAG, "_channelWrite: %d bytes to %s", len, channel->id);

    // TODO: 计算写超时时间

    // 没有处理，返回false
    return false;
}

static int64_t _channelNextTimeout(Channel *channel, void *ctx)
{
    ChannelHandler *thiz = (ChannelHandler *)ctx;

    LOG_D(TAG, "_channelNextTimeout: %d", thiz->idle.allIdleTimeSeconds);

    return thiz->idle.allIdleTimeSeconds * 1000000;
}