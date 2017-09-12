/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpContent.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include "HttpContent.h"
#include <tiny_malloc.h>
#include "tiny_log.h"

#define TAG     "HttpContent"

TINY_LOR
TinyRet HttpContent_Construct(HttpContent *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(HttpContent));

    return TINY_RET_OK;
}

TINY_LOR
TinyRet HttpContent_Dispose(HttpContent *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    if (thiz->buf != NULL)
    {
        tiny_free(thiz->buf);
        thiz->buf = NULL;
        thiz->buf_size = 0;
        thiz->data_size = 0;
    }

    return TINY_RET_E_NOT_IMPLEMENTED;
}

TINY_LOR
TinyRet HttpContent_SetSize(HttpContent *thiz, uint32_t size)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        HttpContent_Dispose(thiz);

        if (size > HTTP_CONTENT_MAX_SIZE)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }

        // jxfengzi@gmail.com, 2017.6.9
        thiz->buf = (char *)tiny_malloc(size + 1);
        if (thiz->buf == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }

        memset(thiz->buf, 0, size + 1);

        thiz->buf_size = size;
    } while (0);

    return ret;
}


TINY_LOR
uint32_t HttpContent_LoadBytes(HttpContent *thiz, Bytes *bytes)
{
    uint32_t unused_size = 0;
    uint32_t length_will_be_read = 0;

    RETURN_VAL_IF_FAIL(thiz, 0);
    RETURN_VAL_IF_FAIL(bytes, 0);

    unused_size= thiz->buf_size - thiz->data_size;
    length_will_be_read = bytes->length - bytes->offset;

    if (unused_size < length_will_be_read)
    {
        LOG_E(TAG, "HttpContent_LoadBytes failed: content size < data size");
        return 0;
    }

    memcpy(thiz->buf + thiz->data_size, bytes->value + bytes->offset, length_will_be_read);
    thiz->data_size += length_will_be_read;
    bytes->offset += length_will_be_read;

    return length_will_be_read;
}