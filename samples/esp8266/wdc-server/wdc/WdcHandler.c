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
#include <TinyBuffer.h>
#include <channel/SocketChannel.h>
#include <codec-http/HttpMessage.h>
#include <codec-http/HttpMessageEncoder.h>
#include "wdc/WdcHandler.h"
#include "wdc/WdcInfo.h"

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

ICACHE_FLASH_ATTR
static TinyRet WdcHandler_Construct(ChannelHandler *thiz);

ICACHE_FLASH_ATTR
static TinyRet WdcHandler_Dispose(ChannelHandler *thiz);

ICACHE_FLASH_ATTR
static void WdcHandler_Delete(ChannelHandler *thiz);

ICACHE_FLASH_ATTR
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len);

ICACHE_FLASH_ATTR
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
            WdcHandler_Dispose(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

ICACHE_FLASH_ATTR
static void WdcHandler_Delete(ChannelHandler *thiz)
{
    printf("WdcHandler_Delete\n");

    WdcHandler_Dispose(thiz);
    tiny_free(thiz);
}

ICACHE_FLASH_ATTR
static TinyRet WdcHandler_Construct(ChannelHandler *thiz)
{
    printf("WdcHandler_Construct\n");

    memset(thiz, 0, sizeof(ChannelHandler));

    strncpy(thiz->name, WdcHandler_Name, CHANNEL_HANDLER_NAME_LEN);
	thiz->onRemove = WdcHandler_Delete;
	thiz->inType = DATA_HTTP_MESSAGE;
    thiz->outType = DATA_HTTP_MESSAGE;
    thiz->channelRead = _channelRead;
    thiz->channelWrite = NULL;

    return TINY_RET_OK;
}

ICACHE_FLASH_ATTR
static TinyRet WdcHandler_Dispose(ChannelHandler *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    printf("WdcHandler_Dispose\n");

    memset(thiz, 0, sizeof(ChannelHandler));

    return TINY_RET_OK;
}

ICACHE_FLASH_ATTR
static void _Output (const uint8_t *data, uint32_t size, void *ctx)
{
    Channel *channel = (Channel *)ctx;
    SocketChannel_StartWrite(channel, DATA_RAW, data, size);
}

ICACHE_FLASH_ATTR
static void sendResponse(ChannelHandler *thiz, Channel *channel, int code, const char *status, const char *type, const char *body)
{
    HttpMessageEncoder encoder;
    TinyBuffer * buffer = NULL;
    HttpMessage response;

    printf("sendResponse: %d %s\n", code, status);

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

        uint32_t length = (uint32_t) ((body == NULL) ? 0 : strlen(body));

        HttpMessage_SetResponse(&response, code, status);
        HttpMessage_SetVersion(&response, 1, 1);
        HttpHeader_Set(&response.header, "Content-Type", type);
        HttpHeader_SetInteger(&response.header, "Content-Length", length);

        if (RET_FAILED(HttpMessageEncoder_Construct(&encoder, &response)))
        {
            printf("HttpMessageEncoder_Construct FAILED\n");
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
        TinyBuffer_Delete(buffer);
    }

    HttpMessage_Dispose(&response);
    HttpMessageEncoder_Dispose(&encoder);

    Channel_Close(channel);
}

ICACHE_FLASH_ATTR
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
