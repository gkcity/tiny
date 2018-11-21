/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpMessageCodec.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <channel/SocketChannel.h>
#include <tiny_log.h>
#include <tiny_malloc.h>
#include <tiny_print_binary.h>
#include "HttpMessageCodec.h"
#include "HttpMessage.h"
#include "HttpMessageEncoder.h"

#define TAG     "HttpMessageCodec"

TINY_LOR
static void printHttpMessage(HttpMessage *message)
{
    const char *contentType = HttpHeader_GetValue (&message->header, "content-type");
    const char *contentLength = HttpHeader_GetValue(&message->header, "content-length");

    if (message->type == HTTP_REQUEST)
    {
        LOG_D(TAG, "%s %s %s/%d.%d",
                message->request_line.method,
                message->request_line.uri,
                message->protocol_identifier,
                message->version.major,
                message->version.minor);
    }
    else
    {
        LOG_D(TAG, "%s/%d.%d %d %s",
              message->protocol_identifier,
              message->version.major,
              message->version.minor,
              message->status_line.code,
              message->status_line.status);
    }

    if (contentType != NULL)
    {
        LOG_D(TAG, "Content-Type: %s", contentType);
    }

    if (contentLength != NULL)
    {
        LOG_D(TAG, "Content-Length: %s", contentLength);
    }

    if (contentType != NULL)
    {
        if (message->content.buf != NULL)
        {
            if (strstr(contentType, "txt") != NULL || strstr(contentType, "json") != NULL)
            {
                LOG_D(TAG, "Content: %s", message->content.buf);
            }
            else
            {
                LOG_BINARY("Content", (const uint8_t *) message->content.buf, message->content.data_size, true);
            }
        }
    }
}

TINY_LOR
static bool _channelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    LOG_D(TAG, "_channelRead");

    //printf("%s\n", (const char *)data);

    do
    {
        if (type != DATA_RAW)
        {
            LOG_D(TAG, "_channelRead inType error: %d", type);
            break;
        }

        if (thiz->context == NULL)
        {
            thiz->context = HttpMessage_New();
            if (thiz->context == NULL)
            {
                LOG_D(TAG, "HttpMessage_New failed");
                break;
            }
        }

        if (RET_FAILED(HttpMessage_Parse((HttpMessage *) thiz->context, data, len)))
        {
            LOG_D(TAG, "HttpMessage_Parse failed!");
            HttpMessage_Delete((HttpMessage *) thiz->context);
            thiz->context = NULL;
            break;
        }

        if (((HttpMessage *) thiz->context)->parser_status == HttpParserError)
        {
            LOG_D(TAG, "HttpParserError");
            HttpMessage_Delete((HttpMessage *) thiz->context);
            thiz->context = NULL;
            break;
        }

        if (HttpMessage_IsContentFull((HttpMessage *) thiz->context))
        {
            printHttpMessage((HttpMessage *) thiz->context);

            SocketChannel_NextRead(channel, DATA_HTTP_MESSAGE, (HttpMessage *) thiz->context, len);
            HttpMessage_Delete((HttpMessage *) thiz->context);
            thiz->context = NULL;
        }
    } while (0);

    return true;
}

static void _Output (const uint8_t *data, uint32_t size, void *ctx)
{
    Channel *channel = (Channel *)ctx;
    SocketChannel_StartWrite(channel, DATA_RAW, data, size);
}

static bool _ChannelWrite(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    HttpMessage *message = (HttpMessage *) data;

    LOG_D(TAG, "_ChannelWrite");

    do
    {
        HttpMessageEncoder encoder;
        ByteBuffer *buffer = NULL;

        if (message == NULL)
        {
            LOG_E(TAG, "_ChannelWrite message is null");
        }

        if (type != thiz->outType)
        {
            LOG_E(TAG, "_ChannelWrite outType error: %d", type);
            break;
        }

        printHttpMessage(message);

        if (RET_FAILED(HttpMessageEncoder_Construct(&encoder, message)))
        {
            LOG_E(TAG, "HttpMessageEncoder_Construct FAILED");
            break;
        }

        buffer = ByteBuffer_New(HttpMessageCodec_BUFFER_SIZE);
        if (buffer == NULL)
        {
            LOG_E(TAG, "ByteBuffer_New FAILED");
            HttpMessageEncoder_Dispose(&encoder);
            break;
        }

        HttpMessageEncoder_Encode(&encoder, buffer, _Output, channel);

        ByteBuffer_Delete(buffer);
        HttpMessageEncoder_Dispose(&encoder);
    } while (0);

    return true;
}

TINY_LOR
static TinyRet HttpMessageCodec_Dispose(ChannelHandler *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    if (thiz->context != NULL)
    {
        HttpMessage_Delete((HttpMessage *) thiz->context);
        thiz->context = NULL;
    }

    memset(thiz, 0, sizeof(ChannelHandler));

    return TINY_RET_OK;
}

TINY_LOR
static void HttpMessageCodec_Delete(ChannelHandler *thiz)
{
    HttpMessageCodec_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
static TinyRet HttpMessageCodec_Construct(ChannelHandler *thiz)
{
    memset(thiz, 0, sizeof(ChannelHandler));

    strncpy(thiz->name, HttpMessageCodec_Name, CHANNEL_HANDLER_NAME_LEN);

    thiz->invalid = false;
    thiz->onRemove = HttpMessageCodec_Delete;
    thiz->inType = DATA_RAW;
    thiz->outType = DATA_HTTP_MESSAGE;
    thiz->channelRead = _channelRead;
    thiz->channelWrite = _ChannelWrite;
    thiz->context = NULL;

    return TINY_RET_OK;
}

TINY_LOR
ChannelHandler *HttpMessageCodec(void)
{
    ChannelHandler *thiz = NULL;

    do
    {
        thiz = (ChannelHandler *) tiny_malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(HttpMessageCodec_Construct(thiz)))
        {
            HttpMessageCodec_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}
