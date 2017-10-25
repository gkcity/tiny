/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   MyClientHandler.c
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
#include "MyClientHandler.h"

#define TAG "MyClientHandler"

static TinyRet MyClientHandler_Construct(ChannelHandler *thiz);
static TinyRet MyClientHandler_Dispose(ChannelHandler *thiz);
static void MyClientHandler_Delete(ChannelHandler *thiz);
static void _channelActive(ChannelHandler *thiz, Channel *channel);
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len);
static void _channelEvent(ChannelHandler *thiz, Channel *channel, ChannelTimer *timer);

ChannelHandler * MyClientHandler(void)
{
    ChannelHandler *thiz = NULL;

    do
    {
        thiz = (ChannelHandler *)tiny_malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(MyClientHandler_Construct(thiz)))
        {
            MyClientHandler_Dispose(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static void MyClientHandler_Delete(ChannelHandler *thiz)
{
    LOG_D(TAG, "MyClientHandler_Delete");

    MyClientHandler_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet MyClientHandler_Construct(ChannelHandler *thiz)
{
    LOG_D(TAG, "MyClientHandler_Construct");

    memset(thiz, 0, sizeof(ChannelHandler));

    strncpy(thiz->name, MyClientHandler_Name, CHANNEL_HANDLER_NAME_LEN);
	thiz->onRemove = MyClientHandler_Delete;
	thiz->inType = DATA_HTTP_MESSAGE;
    thiz->outType = DATA_HTTP_MESSAGE;
    thiz->channelRead = _channelRead;
    thiz->channelWrite = NULL;
    thiz->channelEvent = _channelEvent;
    thiz->channelActive = _channelActive;

    return TINY_RET_OK;
}

static TinyRet MyClientHandler_Dispose(ChannelHandler *thiz)
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

static void _channelActive(ChannelHandler *thiz, Channel *channel)
{
    HttpMessage request;
    HttpMessageEncoder encoder;
    TinyBuffer * buffer = NULL;

    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(channel);

    LOG_D(TAG, "_channelActive");

    do
    {
        buffer = TinyBuffer_New(1024);
        if (buffer == NULL)
        {
            LOG_E(TAG, "TinyBuffer_New FAILED: 1024");
            break;
        }

        if (RET_FAILED(HttpMessage_Construct(&request)))
        {
            LOG_E(TAG, "HttpMessage_Construct FAILED");
            break;
        }

        HttpMessage_SetRequest(&request, "GET", "/homekit/instance/device?type=urn:homtkit-spec:device:lightbulb:00000000:arrizo-v1");
        HttpHeader_Set(&request.header, "Accept", "*/*");
        HttpHeader_SetHost(&request.header, "47.93.60.147", 8080);

        if (RET_FAILED(HttpMessageEncoder_Construct(&encoder, &request)))
        {
            LOG_E(TAG, "HttpMessageEncoder_Construct FAILED");
            break;
        }

        HttpMessageEncoder_Encode(&encoder, buffer, _Output, channel);
    } while (false);

    if (buffer != NULL)
    {
        TinyBuffer_Delete(buffer);
    }

    HttpMessage_Dispose(&request);
    HttpMessageEncoder_Dispose(&encoder);
}

static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    HttpMessage *response = (HttpMessage *)data;

    LOG_D(TAG, "_channelRead: %d %s", response->status_line.code, response->status_line.status);

    printf("content length: %d\n", response->content.buf_size);
    printf("content: %s\n", response->content.buf);


    //    if (RET_SUCCEEDED(HttpMessage_Construct(&response)))
//    {
//        HttpMessage_SetResponse(&response, 200, "OK");
//        HttpMessage_SetVersion(&response, 1, 1);
//
//        HttpHeader_Set(&response.header, "Content-Type", "text/json");
//        HttpHeader_SetInteger(&response.header, "Content-Length", 0);
//
//        SocketChannel_StartWrite(channel, DATA_RAW, HttpMessage_GetBytesWithoutContent(&response), HttpMessage_GetBytesSizeWithoutContent(&response));
//
//		if (response.content.buf != NULL)
//		{
//			SocketChannel_StartWrite(channel, DATA_RAW, response.content.buf, response.content.buf_size);
//		}
//
//        Channel_Close(channel);
//        HttpMessage_Dispose(&response);
//    }

//    Channel_Close(channel);

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

    //    printf("connection is timeout, close the connection\n");
    //    Channel_Close(channel);
}