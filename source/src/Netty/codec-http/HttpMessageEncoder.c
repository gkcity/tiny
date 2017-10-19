/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpMessageEncoder.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include <tiny_log.h>
#include <tiny_snprintf.h>
#include <TinyMapItem.h>
#include "HttpMessageEncoder.h"

#define TAG                 "HttpMessageEncoder"

TINY_LOR
static void _Append(HttpMessageEncoder *thiz, const uint8_t *data, uint32_t size)
{
    if (thiz->buffer == NULL)
    {
        thiz->output(data, size, thiz->ctx);
    }
    else
    {
        uint32_t offset = 0;

        while (size > 0)
        {
            uint32_t copied = TinyBuffer_Add(thiz->buffer, data, offset, size);
            offset += copied;
            size -= copied;

            if (TinyBuffer_IsFull(thiz->buffer))
            {
                thiz->out += thiz->buffer->used;
                thiz->output(thiz->buffer->bytes, thiz->buffer->size, thiz->ctx);
                TinyBuffer_Clear(thiz->buffer);
            }
            else
            {
                if (thiz->size == thiz->out + thiz->buffer->used)
                {
                    thiz->out += thiz->buffer->used;
                    thiz->output(thiz->buffer->bytes, thiz->buffer->used, thiz->ctx);
                }
            }
        }
    }
}

TINY_LOR
static void _Encode(HttpMessageEncoder *thiz, HttpMessage *message)
{
    char version[8];

    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(message);

    memset(version, 0, 8);
    tiny_snprintf(version, 8, "/%d.%d", message->version.major, message->version.minor);

    if (message->type == HTTP_REQUEST)
    {
        _Append(thiz, (uint8_t *)message->request_line.method, strlen(message->request_line.method));
        _Append(thiz, (uint8_t *)" ", 1);
        _Append(thiz, (uint8_t *)message->request_line.uri, strlen(message->request_line.uri));
        _Append(thiz, (uint8_t *)" ", 1);
        _Append(thiz, (uint8_t *)message->protocol_identifier, strlen(message->protocol_identifier));
        _Append(thiz, (uint8_t *)version, strlen(version));
        _Append(thiz, (uint8_t *)"\r\n", 2);
    }
    else
    {
        char code[4];

        memset(code, 0, 4);
        tiny_snprintf(code, 4, "%d", message->status_line.code);

        _Append(thiz, (uint8_t *)message->protocol_identifier, strlen(message->protocol_identifier));
        _Append(thiz, (uint8_t *)version, strlen(version));
        _Append(thiz, (uint8_t *)" ", 1);
        _Append(thiz, (uint8_t *)code, strlen(code));
        _Append(thiz, (uint8_t *)" ", 1);
        _Append(thiz, (uint8_t *)message->status_line.status, strlen(message->status_line.status));
        _Append(thiz, (uint8_t *)"\r\n", 2);
    }

    for (uint32_t i = 0; i < message->header.values.list.size; ++i)
    {
        TinyMapItem *item = (TinyMapItem *) TinyList_GetAt(&message->header.values.list, i);

        _Append(thiz, (uint8_t *)item->key, strlen(item->key));
        _Append(thiz, (const uint8_t *)": ", 2);
        _Append(thiz, (uint8_t *)item->value, strlen(item->value));
        _Append(thiz, (uint8_t *)"\r\n", 2);
    }

    _Append(thiz, (uint8_t *)"\r\n", 2);

    if (message->content.buf != NULL)
    {
        _Append(thiz, (uint8_t *)message->content.buf, message->content.buf_size);
    }
}

TINY_LOR
static void _Count (const uint8_t *data, uint32_t size, void *ctx)
{
    HttpMessageEncoder *thiz = (HttpMessageEncoder *)ctx;
    thiz->size += size;
}

TINY_LOR
TinyRet HttpMessageEncoder_Construct(HttpMessageEncoder *thiz, HttpMessage *message)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(message, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(HttpMessageEncoder));

        if (message->type == HTTP_UNDEFINED)
        {
            LOG_D(TAG, "HTTP TYPE invalid");
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        thiz->message = message;
        thiz->buffer = NULL;

        thiz->output = _Count;
        thiz->ctx = thiz;
        thiz->size = 0;
        thiz->out = 0;

        _Encode(thiz, message);

        thiz->output = NULL;
        thiz->ctx = NULL;
    } while (false);

    return ret;
}

TINY_LOR
void HttpMessageEncoder_Dispose(HttpMessageEncoder *thiz)
{

}

TINY_LOR
void HttpMessageEncoder_Encode(HttpMessageEncoder *thiz, TinyBuffer *buffer, HttpMessageOutput output, void *ctx)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(output);

    thiz->buffer = buffer;
    thiz->output = output;
    thiz->ctx = ctx;
    
    if (thiz->buffer != NULL)
    {
        TinyBuffer_Clear(thiz->buffer);
    }

    _Encode(thiz, thiz->message);
}