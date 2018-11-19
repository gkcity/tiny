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
            SocketChannel_NextRead(channel, DATA_HTTP_MESSAGE, (HttpMessage *) thiz->context, len);
            HttpMessage_Delete((HttpMessage *) thiz->context);
            thiz->context = NULL;
        }
    } while (0);

    return true;
}

static void _OutputTXT (const uint8_t *data, uint32_t size, void *ctx)
{
    LOG_D(TAG, "_Output (%d)", size);
    Channel *channel = (Channel *)ctx;
    SocketChannel_StartWrite(channel, DATA_RAW, data, size);
}

#if 0
static void _OutputBINARY (const uint8_t *data, uint32_t size, void *ctx)
{
    LOG_D(TAG, "_Output (%d):", size);
    LOG_BINARY(TAG, data, size, true);

    Channel *channel = (Channel *)ctx;
    SocketChannel_StartWrite(channel, DATA_RAW, data, size);
}
#endif

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

        HttpMessageEncoder_Encode(&encoder, buffer, _OutputTXT, channel);

#if 0
        const char *contentType = (const char *)TinyMap_GetValue(&message->header.values, "Content-Type");
        if (contentType == NULL)
        {
            HttpMessageEncoder_Encode(&encoder, buffer, _OutputTXT, channel);
        }
        else
        {
            if (strstr(contentType, "txt") != NULL)
            {
                HttpMessageEncoder_Encode(&encoder, buffer, _OutputTXT, channel);
            }
            else if (strstr(contentType, "json") != NULL)
            {
                HttpMessageEncoder_Encode(&encoder, buffer, _OutputTXT, channel);
            }
            else
            {
                HttpMessageEncoder_Encode(&encoder, buffer, _OutputBINARY, channel);
            }
        };
#endif

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
