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

HttpContent * HttpContent_New(void)
{
    HttpContent *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (HttpContent *)tiny_malloc(sizeof(HttpContent));
        if (thiz == NULL)
        {
            break;
        }

        ret = HttpContent_Construct(thiz);
        if (RET_FAILED(ret))
        {
            HttpContent_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet HttpContent_Construct(HttpContent *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(HttpContent));

    return TINY_RET_OK;
}

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

void HttpContent_Delete(HttpContent *thiz)
{
    RETURN_IF_FAIL(thiz);

    tiny_free(thiz);
}

void HttpContent_Copy(HttpContent *dst, HttpContent *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    dst->buf_size = src->buf_size;
    dst->data_size = src->data_size;

    if (src->buf_size > 0)
    {
        dst->buf = (char *)tiny_malloc(dst->buf_size);
        if (dst->buf != NULL)
        {
            memset(dst->buf, 0, dst->buf_size);
            memcpy(dst->buf, src->buf, src->buf_size);
        }
        else
        {
            LOG_E(TAG, "out of memory!");
        }
    }
}

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

uint32_t HttpContent_AddBytes(HttpContent *thiz, const char *data, uint32_t size)
{
    uint32_t unused_size = thiz->buf_size - thiz->data_size;

    RETURN_VAL_IF_FAIL(thiz, 0);
    RETURN_VAL_IF_FAIL(data, 0);

    if (unused_size < size)
    {
        LOG_E(TAG, "HttpContent_AddBytes failed: content size < data size");
        return 0;
    }

    memcpy(thiz->buf + thiz->data_size, data, size);
    thiz->data_size += size;

    return size;
}

bool HttpContent_IsFull(HttpContent *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return (thiz->buf_size == thiz->data_size);
}

uint32_t HttpContent_GetSize(HttpContent * thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->buf_size;
}

const char * HttpContent_GetObject(HttpContent * thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->buf;
}