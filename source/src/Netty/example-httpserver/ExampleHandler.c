/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ExampleHandler.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <codec-http/HttpMessage.h>
#include <codec-http/HttpMessageEncoder.h>
#include <channel/SocketChannel.h>
#include "ExampleHandler.h"

#define TAG "ExampleHandler"

static TinyRet ExampleHandler_Construct(ChannelHandler *thiz);
static TinyRet ExampleHandler_Dispose(ChannelHandler *thiz);
static void ExampleHandler_Delete(ChannelHandler *thiz);
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len);
static void _channelEvent(ChannelHandler *thiz, Channel *channel, ChannelTimer *timer);

ChannelHandler * ExampleHandler(void)
{
    ChannelHandler *thiz = NULL;

    do
    {
        thiz = (ChannelHandler *)tiny_malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(ExampleHandler_Construct(thiz)))
        {
            ExampleHandler_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static void ExampleHandler_Delete(ChannelHandler *thiz)
{
    LOG_D(TAG, "ExampleHandler_Delete");

    ExampleHandler_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet ExampleHandler_Construct(ChannelHandler *thiz)
{
    LOG_D(TAG, "ExampleHandler_Construct");

    memset(thiz, 0, sizeof(ChannelHandler));

    strncpy(thiz->name, ExampleHandler_Name, CHANNEL_HANDLER_NAME_LEN);
	thiz->onRemove = ExampleHandler_Delete;
	thiz->inType = DATA_HTTP_MESSAGE;
    thiz->outType = DATA_HTTP_MESSAGE;
    thiz->channelRead = _channelRead;
    thiz->channelWrite = NULL;
    thiz->channelEvent = _channelEvent;

    return TINY_RET_OK;
}

static TinyRet ExampleHandler_Dispose(ChannelHandler *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(ChannelHandler));

    return TINY_RET_OK;
}

#define BODY "{\"code\": 12345}"

static void _Output (const uint8_t *data, uint32_t size, void *ctx)
{
    Channel *channel = (Channel *)ctx;
    SocketChannel_StartWrite(channel, DATA_RAW, data, size);
}

static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    HttpMessage *request = (HttpMessage *)data;
    HttpMessage response;
    HttpMessageEncoder encoder;
    ByteBuffer * buffer = NULL;

    LOG_D(TAG, "_channelRead: %s %s", request->request_line.method, request->request_line.uri);

    do
    {
        buffer = ByteBuffer_New(1024);
        if (buffer == NULL)
        {
            LOG_E(TAG, "ByteBuffer_New FAILED: 1024");
            break;
        }

        if (RET_FAILED(HttpMessage_Construct(&response)))
        {
            LOG_E(TAG, "HttpMessage_Construct FAILED");
            break;
        }

        uint32_t length = (uint32_t) strlen(BODY);

        HttpMessage_SetResponse(&response, 200, "OK");
        HttpMessage_SetVersion(&response, 1, 1);
        HttpHeader_Set(&response.header, "Content-Type", "text/json");
        HttpHeader_SetInteger(&response.header, "Content-Length", length);

        if (RET_FAILED(HttpMessageEncoder_Construct(&encoder, &response)))
        {
            LOG_E(TAG, "HttpMessageEncoder_Construct FAILED");
            break;
        }

        HttpMessageEncoder_Encode(&encoder, buffer, _Output, channel);

        SocketChannel_StartWrite(channel, DATA_RAW, BODY, length);
    } while (false);

    if (buffer != NULL)
    {
        ByteBuffer_Delete(buffer);
    }

    HttpMessage_Dispose(&response);
    HttpMessageEncoder_Dispose(&encoder);

    return true;
}

static void _channelEvent(ChannelHandler *thiz, Channel *channel, ChannelTimer *timer)
{
    LOG_D(TAG, "_channelEvent: %s", channel->id);

    Channel_Close(channel);
}