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

#include <codec-http/HttpMessage.h>
#include <codec-http/HttpMessageEncoder.h>
#include <channel/SocketChannel.h>
#include "ExampleHandler.h"

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
        thiz = (ChannelHandler *)malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(ExampleHandler_Construct(thiz)))
        {
            ExampleHandler_Dispose(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static void ExampleHandler_Delete(ChannelHandler *thiz)
{
    ExampleHandler_Dispose(thiz);
    free(thiz);
}

static TinyRet ExampleHandler_Construct(ChannelHandler *thiz)
{
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

static void _Output (const uint8_t *data, uint32_t size, void *ctx)
{
    Channel *channel = (Channel *)ctx;
    SocketChannel_StartWrite(channel, DATA_RAW, data, size);
}

static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    HttpMessage *request = (HttpMessage *)data;
    HttpMessageEncoder encoder;
    HttpMessage response;
    TinyBuffer * buffer = NULL;

    printf("_channelRead: %s %s\n", request->request_line.method, request->request_line.uri);
    printf("write response to: %s\n", channel->id);

#if 0
    const char * resp = "HTTP/1.1 200 OK\r\nContent-Type: text/json\r\nContent-Length: 0\r\n\r\n";
    lwip_send(channel->fd, resp, strlen(resp), 0);
    Channel_Close(channel);
#else

    do
    {
        buffer = TinyBuffer_New(1024);
        if (buffer == NULL)
        {
            printf("TinyBuffer_New FAILED: 1024\n");
            break;
        }

        if (RET_FAILED(HttpMessage_Construct(&response)))
        {
            printf("HttpMessage_Construct FAILED\n");
            break;
        }

        HttpMessage_SetResponse(&response, 200, "OK");
        HttpMessage_SetVersion(&response, 1, 1);
        HttpHeader_Set(&response.header, "Content-Type", "text/json");
        HttpHeader_SetInteger(&response.header, "Content-Length", 0);

        if (RET_FAILED(HttpMessageEncoder_Construct(&encoder, &response)))
        {
            printf("HttpMessageEncoder_Construct FAILED");
            break;
        }

        HttpMessageEncoder_Encode(&encoder, buffer, _Output, channel);
    } while (false);

    if (buffer != NULL)
    {
        TinyBuffer_Delete(buffer);
    }

    HttpMessage_Dispose(&response);
    HttpMessageEncoder_Dispose(&encoder);
#endif

    return true;
}

static void _channelEvent(ChannelHandler *thiz, Channel *channel, ChannelTimer *timer)
{
    printf("_channelEvent: %s\n", channel->id);

    Channel_Close(channel);
}
