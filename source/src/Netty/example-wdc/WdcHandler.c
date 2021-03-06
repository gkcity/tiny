/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WdcHandler.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <ByteBuffer.h>
#include <channel/SocketChannel.h>
#include <codec-http/HttpMessage.h>
#include <codec-http/HttpMessageEncoder.h>
#include "WdcHandler.h"

#define TAG "WdcHandler"

#define SETTINGS_HTML "\
<html>\
<head>\
<title>Wireless Device Configuration</title>\
</head>\
<body>\
<form>\
SSID:<br>\
<input type=\"text\" name=\"ssid\" value=\"airport-ouyang\" size=\"32\">\
<br>\
<br>\
PASSWORD:<br>\
<input type=\"text\" name=\"password\" value=\"hello\" size=\"32\">\
<br>\
<br>\
<input type=\"submit\" value=\"OK\">\
</form>\
</body>\
</html>"

#define OK_HTML "\
<html>\
<head>\
<title>Wireless Device Configuration</title>\
</head>\
<body>\
OK!\
</body>\
</html>"

static TinyRet WdcHandler_Construct(ChannelHandler *thiz);
static TinyRet WdcHandler_Dispose(ChannelHandler *thiz);
static void WdcHandler_Delete(ChannelHandler *thiz);
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len);
static void _channelEvent(ChannelHandler *thiz, Channel *channel, ChannelTimer *timer);

ChannelHandler * WdcHandler(void)
{
    ChannelHandler *thiz = NULL;

    do
    {
        thiz = (ChannelHandler *)tiny_malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(WdcHandler_Construct(thiz)))
        {
            WdcHandler_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static void WdcHandler_Delete(ChannelHandler *thiz)
{
    LOG_D(TAG, "WdcHandler_Delete");

    WdcHandler_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet WdcHandler_Construct(ChannelHandler *thiz)
{
    LOG_D(TAG, "WdcHandler_Construct");

    memset(thiz, 0, sizeof(ChannelHandler));

    strncpy(thiz->name, WdcHandler_Name, CHANNEL_HANDLER_NAME_LEN);
	thiz->onRemove = WdcHandler_Delete;
	thiz->inType = DATA_HTTP_MESSAGE;
    thiz->outType = DATA_HTTP_MESSAGE;
    thiz->channelRead = _channelRead;
    thiz->channelWrite = NULL;
    thiz->channelEvent = _channelEvent;

    return TINY_RET_OK;
}

TINY_LOR
static TinyRet WdcHandler_Dispose(ChannelHandler *thiz)
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
static void sendResponse(ChannelHandler *thiz, Channel *channel, int code, const char *status, const char *type, const char *body)
{
    HttpMessageEncoder encoder;
    ByteBuffer * buffer = NULL;
    HttpMessage response;

    printf("sendResponse: %d %s\n", code, status);

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

        uint32_t length = (uint32_t) ((body == NULL) ? 0 : strlen(body));

        HttpMessage_SetResponse(&response, code, status);
        HttpMessage_SetVersion(&response, 1, 1);
        HttpHeader_Set(&response.header, "Content-Type", type);
        HttpHeader_SetInteger(&response.header, "Content-Length", length);

        if (RET_FAILED(HttpMessageEncoder_Construct(&encoder, &response)))
        {
            LOG_E(TAG, "HttpMessageEncoder_Construct FAILED");
            break;
        }

        HttpMessageEncoder_Encode(&encoder, buffer, _Output, channel);

        if (body != NULL)
        {
            SocketChannel_StartWrite(channel, DATA_RAW, body, length);
        }

    } while (false);

    if (buffer != NULL)
    {
        ByteBuffer_Delete(buffer);
    }

    HttpMessage_Dispose(&response);
    HttpMessageEncoder_Dispose(&encoder);

    Channel_Close(channel);
}

TINY_LOR
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    printf("_channelRead: ");

    HttpMessage *request = (HttpMessage *)data;

    printf("%s %s\n", request->request_line.method, request->request_line.uri);

    // uri: /wdc?ssid=airport&password=hello

    do
    {
        char path[5];
        const char *start = NULL;
        const char *end = NULL;

        memset(path, 0, 5);

        if (strcmp("/wdc", request->request_line.uri) == 0)
        {
            sendResponse(thiz, channel, 200, "OK", "text/html", SETTINGS_HTML);
            break;
        }

        start = request->request_line.uri;
        end = strstr(request->request_line.uri, "?");
        if (end == NULL)
        {
            sendResponse(thiz, channel, 400, "BAD REQUEST", "text/html", NULL);
            break;
        }

        if ((end - start) != 4)
        {
            sendResponse(thiz, channel, 400, "BAD REQUEST", "text/html", NULL);
            break;
        }

        strncpy(path, start, 4);
        printf("path: %s\n", path);

        if (strcmp(path, "/wdc") != 0)
        {
            sendResponse(thiz, channel, 400, "BAD REQUEST", "text/html", NULL);
            break;
        }

        start = end + 1;
        end = strstr(start, "ssid=");
        if (end != start)
        {
            sendResponse(thiz, channel, 400, "BAD REQUEST", "text/html", NULL);
            break;
        }

        start = end + strlen("ssid=");
        end = strstr(start, "&");
        if (end == NULL)
        {
            sendResponse(thiz, channel, 400, "BAD REQUEST", "text/html", NULL);
            break;
        }

        start = end + 1;
        end = strstr(start, "password=");
        if (end != start)
        {
            sendResponse(thiz, channel, 400, "BAD REQUEST", "text/html", NULL);
            break;
        }

        start = end + strlen("password=");
        if (strlen(start) > 32)
        {
            sendResponse(thiz, channel, 400, "BAD REQUEST", "text/html", NULL);
            break;
        }

        sendResponse(thiz, channel, 200, "OK", "text/html", OK_HTML);
    } while (0);

    return true;
}

static void _channelEvent(ChannelHandler *thiz, Channel *channel, ChannelTimer *timer)
{
    LOG_D(TAG, "_channelEvent: %s", channel->id);

    Channel_Close(channel);
}