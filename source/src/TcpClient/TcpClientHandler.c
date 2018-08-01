/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpClientHandler.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <TinyMapItem.h>
#include <TinyBuffer.h>
#include <channel/SocketChannel.h>
#include "TcpClientHandler.h"

#define TAG                 "TcpClientHandler"

TINY_LOR
static TinyRet TcpClientHandler_Construct(ChannelHandler *thiz, TcpExchange *context);

TINY_LOR
static TinyRet TcpClientHandler_Dispose(ChannelHandler *thiz);

TINY_LOR
static void TcpClientHandler_Delete(ChannelHandler *thiz);

TINY_LOR
static void _channelActive(ChannelHandler *thiz, Channel *channel);

TINY_LOR
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len);

TINY_LOR
static void _channelEvent(ChannelHandler *thiz, Channel *channel, ChannelTimer *timer);

TINY_LOR
ChannelHandler * TcpClientHandler(TcpExchange *context)
{
    ChannelHandler *thiz = NULL;

    do
    {
        thiz = (ChannelHandler *)tiny_malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(TcpClientHandler_Construct(thiz, context)))
        {
            TcpClientHandler_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
static void TcpClientHandler_Delete(ChannelHandler *thiz)
{
    LOG_D(TAG, "TcpClientHandler_Delete");

    TcpClientHandler_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
static TinyRet TcpClientHandler_Construct(ChannelHandler *thiz, TcpExchange *context)
{
    LOG_D(TAG, "TcpClientHandler_Construct");

    memset(thiz, 0, sizeof(ChannelHandler));

    strncpy(thiz->name, TcpClientHandler_Name, CHANNEL_HANDLER_NAME_LEN);
	thiz->onRemove = TcpClientHandler_Delete;
	thiz->inType = DATA_RAW;
    thiz->outType = DATA_RAW;
    thiz->channelRead = _channelRead;
    thiz->channelWrite = NULL;
    thiz->channelEvent = _channelEvent;
    thiz->channelActive = _channelActive;
    thiz->context = context;

    return TINY_RET_OK;
}

TINY_LOR
static TinyRet TcpClientHandler_Dispose(ChannelHandler *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(ChannelHandler));

    return TINY_RET_OK;
}

static void _Output (const uint8_t *data, uint32_t size, void *ctx)
{
    Channel *channel = (Channel *)ctx;
    SocketChannel_StartWrite(channel, DATA_RAW, data, size);
}

TINY_LOR
static void _channelActive(ChannelHandler *thiz, Channel *channel)
{
    TcpExchange *exchange = (TcpExchange *)thiz->context;

    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(channel);

    LOG_I(TAG, "_channelActive");

    SocketChannel_StartWrite(channel, DATA_RAW, exchange->content, exchange->length);
}

TINY_LOR
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    TcpExchange *exchange = (TcpExchange *)thiz->context;

    LOG_I(TAG, "_channelRead: %d", len);

    exchange->length = 0;
    if (exchange->content != NULL)
    {
        tiny_free(exchange->content);
        exchange->content = NULL;
    }

    exchange->content = tiny_malloc(len + 1);
    if (exchange->content != NULL)
    {
        memset(exchange->content, 0, len + 1);
        memcpy(exchange->content, data, len);
        exchange->length = len;
    }
    else
    {
        LOG_E(TAG, "tiny_malloc FAILED!");
    }

    Channel_Close(channel);

    return true;
}

TINY_LOR
static void _channelEvent(ChannelHandler *thiz, Channel *channel, ChannelTimer *timer)
{
    LOG_E(TAG, "_channelEvent: %s", channel->id);

    switch (timer->type)
    {
        case CHANNEL_TIMER_READER:
            LOG_E(TAG, "IDLE_READER");
            break;

        case CHANNEL_TIMER_WRITER:
            LOG_E(TAG, "IDLE_WRITER");
            break;

        case CHANNEL_TIMER_ALL:
            LOG_E(TAG, "IDLE_ALL");
            break;

        default:
            break;
    }

    LOG_D(TAG, "connection is timeout, close the connection!");
    Channel_Close(channel);
}