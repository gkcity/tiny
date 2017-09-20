/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpClientHandler.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <codec-http/HttpMessage.h>
#include <channel/SocketChannel.h>
#include "HttpClientHandler.h"

#define TAG "HttpClientHandler"

static TinyRet HttpClientHandler_Construct(ChannelHandler *thiz, HttpExchange *context);
static TinyRet HttpClientHandler_Dispose(ChannelHandler *thiz);
static void HttpClientHandler_Delete(ChannelHandler *thiz);
static void _channelActive(ChannelHandler *thiz, Channel *channel);
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len);
static void _channelEvent(ChannelHandler *thiz, Channel *channel, ChannelTimer *timer);

TINY_LOR
ChannelHandler * HttpClientHandler(HttpExchange *context)
{
    ChannelHandler *thiz = NULL;

    do
    {
        thiz = (ChannelHandler *)tiny_malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(HttpClientHandler_Construct(thiz, context)))
        {
            HttpClientHandler_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static void HttpClientHandler_Delete(ChannelHandler *thiz)
{
    LOG_D(TAG, "HttpClientHandler_Delete");

    HttpClientHandler_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet HttpClientHandler_Construct(ChannelHandler *thiz, HttpExchange *context)
{
    LOG_D(TAG, "HttpClientHandler_Construct");

    memset(thiz, 0, sizeof(ChannelHandler));

    strncpy(thiz->name, HttpClientHandler_Name, CHANNEL_HANDLER_NAME_LEN);
	thiz->onRemove = HttpClientHandler_Delete;
	thiz->inType = DATA_HTTP_MESSAGE;
    thiz->outType = DATA_HTTP_MESSAGE;
    thiz->channelRead = _channelRead;
    thiz->channelWrite = NULL;
    thiz->channelEvent = _channelEvent;
    thiz->channelActive = _channelActive;
    thiz->data = context;

    return TINY_RET_OK;
}

static TinyRet HttpClientHandler_Dispose(ChannelHandler *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(ChannelHandler));

    return TINY_RET_OK;
}

static void _channelActive(ChannelHandler *thiz, Channel *channel)
{
    HttpExchange *exchange = (HttpExchange *)thiz->data;
    HttpMessage request;

    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(channel);

    LOG_D(TAG, "_channelActive");

    if (RET_SUCCEEDED(HttpMessage_Construct(&request)))
    {
        HttpMessage_SetRequest(&request, exchange->method, exchange->uri);
        HttpHeader_Set(&request.header, "Accept", "*/*");
        HttpHeader_SetHost(&request.header, exchange->ip, exchange->port);

        SocketChannel_StartWrite(channel, DATA_RAW, HttpMessage_GetBytesWithoutContent(&request), HttpMessage_GetBytesSizeWithoutContent(&request));
        HttpMessage_Dispose(&request);
    }
}

static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    HttpMessage *response = (HttpMessage *)data;
    HttpExchange *exchange = (HttpExchange *)thiz->data;

    LOG_D(TAG, "_channelRead: %d %s", response->status_line.code, response->status_line.status);

    exchange->status = response->status_line.code;
    if (exchange->status == HTTP_STATUS_OK)
    {
        exchange->length = response->content.buf_size;
        if (exchange->length > 0)
        {
            exchange->content = tiny_malloc(exchange->length + 1);
            if (exchange->content != NULL)
            {
                memset(exchange->content, 0, exchange->length + 1);
                memcpy(exchange->content, response->content.buf, exchange->length);
            }
            else
            {
                LOG_E(TAG, "tiny_malloc FAILED!");
            }
        }
    }

    Channel_Close(channel);

    return true;
}

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