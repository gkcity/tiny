/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyBuffer.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "TinyBuffer.h"
#include <tiny_print_mem.h>

#define TAG     "TinyBuffer"

TINY_LOR
TinyBuffer * TinyBuffer_New(uint32_t size)
{
    TinyBuffer * thiz = NULL;

    do
    {
        thiz = (TinyBuffer *) tiny_malloc(sizeof(TinyBuffer));
        if (thiz == NULL)
        {
            LOG_D(TAG, "tiny_malloc FAILED!");
            LOG_MEM(TAG, "TinyBuffer_New");
            break;
        }

        if (RET_FAILED(TinyBuffer_Construct(thiz, size)))
        {
            LOG_E(TAG, "TinyBuffer_Construct FAILED");
            TinyBuffer_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TINY_LOR
void TinyBuffer_Delete(TinyBuffer *thiz)
{
    TinyBuffer_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
TinyRet TinyBuffer_Construct(TinyBuffer *thiz, uint32_t size)
{
    TinyRet ret = TINY_RET_OK;

    LOG_I(TAG, "TinyBuffer_Construct: %d", size);

    do
    {
        memset(thiz, 0, sizeof(TinyBuffer));

        if (thiz->bytes != NULL)
        {
            tiny_free(thiz->bytes);
            thiz->bytes = NULL;
        }

        if (size == 0)
        {
            thiz->size = size;
            thiz->used = 0;
            break;
        }

        thiz->bytes = tiny_malloc(size + 1);
        if (thiz->bytes == NULL)
        {
            LOG_E(TAG, "tiny_malloc failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        memset(thiz->bytes, 0, size + 1);
        thiz->size = size;
        thiz->used = 0;
    } while (false);

    return ret;
}

TINY_LOR
void TinyBuffer_Dispose(TinyBuffer *thiz)
{
    if (thiz->bytes != NULL)
    {
        tiny_free(thiz->bytes);
    }
}

TINY_LOR
uint32_t TinyBuffer_Add(TinyBuffer *thiz, const uint8_t *data, uint32_t offset, uint32_t size)
{
    uint32_t available = thiz->size - thiz->used;
    uint32_t copied = available > size ? size : available;

    memcpy(thiz->bytes + thiz->used, data + offset, copied);
    thiz->used += copied;

    return copied;
}

TINY_LOR
void TinyBuffer_Clear(TinyBuffer *thiz)
{
    memset(thiz->bytes, 0, thiz->size + 1);
    thiz->used = 0;
}