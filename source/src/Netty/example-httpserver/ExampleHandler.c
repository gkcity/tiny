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

static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    HttpMessage *request = (HttpMessage *)data;
    HttpMessage response;

    LOG_D(TAG, "_channelRead: %s %s", request->request_line.method, request->request_line.uri);

    if (RET_SUCCEEDED(HttpMessage_Construct(&response)))
    {
        HttpMessage_SetResponse(&response, 200, "OK");
        HttpMessage_SetVersion(&response, 1, 1);

        HttpHeader_Set(&response.header, "Content-Type", "text/json");
        HttpHeader_SetInteger(&response.header, "Content-Length", 0);

        SocketChannel_StartWrite(channel, DATA_RAW, HttpMessage_GetBytesWithoutContent(&response), HttpMessage_GetBytesSizeWithoutContent(&response));

		if (response.content.buf != NULL) 
		{
			SocketChannel_StartWrite(channel, DATA_RAW, response.content.buf, response.content.buf_size);
		}

//        Channel_Close(channel);
        HttpMessage_Dispose(&response);
    }

    return true;
}

static void _channelEvent(ChannelHandler *thiz, Channel *channel, ChannelTimer *timer)
{
    LOG_D(TAG, "_channelEvent: %s", channel->id);

    Channel_Close(channel);
}