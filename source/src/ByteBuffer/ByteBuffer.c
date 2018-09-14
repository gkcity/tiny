/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ByteBuffer.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "ByteBuffer.h"

#define TAG     "ByteBuffer"

TINY_LOR
ByteBuffer * ByteBuffer_New(uint32_t size)
{
    ByteBuffer *thiz = NULL;

    do
    {
        thiz = (ByteBuffer *)tiny_malloc(sizeof(ByteBuffer));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(ByteBuffer_Construct(thiz, size)))
        {
            LOG_E(TAG, "ByteBuffer_Construct FAILED");
            ByteBuffer_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;}

TINY_LOR
void ByteBuffer_Delete(ByteBuffer *thiz)
{
    ByteBuffer_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
TinyRet ByteBuffer_Construct(ByteBuffer *thiz, uint32_t size)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        memset(thiz, 0, sizeof(ByteBuffer));

        if (size == 0)
        {
            LOG_E(TAG, "size is 0");
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        thiz->bytes = tiny_malloc(size);
        if (thiz->bytes == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED: %d", size);
            ret = TINY_RET_E_NEW;
            break;
        }

        memset(thiz->bytes, 0, size);
        thiz->size = size;
    } while (false);

    return ret;
}

TINY_LOR
TinyRet ByteBuffer_Dispose(ByteBuffer *thiz)
{
    if (thiz->bytes != NULL)
    {
        tiny_free(thiz->bytes);
    }

    memset(thiz, 0, sizeof(ByteBuffer));

    return TINY_RET_OK;
}

TINY_LOR
void ByteBuffer_Clear(ByteBuffer *thiz)
{
    thiz->offset = 0;
    thiz->available = 0;
}

TINY_LOR
bool ByteBuffer_Available(ByteBuffer *thiz)
{
    return (thiz->available > 0);
}

TINY_LOR
bool ByteBuffer_Put(ByteBuffer *thiz, uint8_t *data, uint32_t length)
{
    uint32_t unused = thiz->size - thiz->available;
    if (length > unused)
    {
        return false;
    }

    if (thiz->offset != 0)
    {
        if (thiz->available > 0)
        {
            memcpy(thiz->bytes, thiz->bytes + thiz->offset, (size_t) thiz->available);
        }

        thiz->offset = 0;
    }

    memcpy(thiz->bytes + thiz->available, data, length);
    thiz->available += length;

    return true;
}

TINY_LOR
uint32_t ByteBuffer_Add(ByteBuffer *thiz, const uint8_t *data, uint32_t offset, uint32_t length)
{
    uint32_t unused = thiz->size - thiz->available;
    uint32_t copied = unused > length ? length : unused;

    if (unused == 0)
    {
        return 0;
    }

    if (thiz->offset != 0)
    {
        if (thiz->available > 0)
        {
            memcpy(thiz->bytes, thiz->bytes + thiz->offset, (size_t) thiz->available);
        }

        thiz->offset = 0;
    }

    memcpy(thiz->bytes + thiz->available, data + offset, copied);
    thiz->available += copied;

    return copied;
}

TINY_LOR
bool ByteBuffer_Get(ByteBuffer *thiz, uint32_t skipped, uint8_t bytes[], uint32_t length)
{
    bool ret = false;

    RETURN_VAL_IF_FAIL(thiz, false);
    RETURN_VAL_IF_FAIL(length, false);

    ret = ByteBuffer_Pick(thiz, skipped, bytes, length);
    if (ret)
    {
        thiz->offset = thiz->offset + skipped + length;
        if (thiz->offset >= thiz->size)
        {
            thiz->offset -= thiz->size;
        }

        thiz->available -= skipped + length;
        if (thiz->available == 0)
        {
            thiz->offset = 0;
        }
    }

    return ret;
}

TINY_LOR
bool ByteBuffer_Pick(ByteBuffer *thiz, uint32_t skipped, uint8_t bytes[], uint32_t length)
{
    uint32_t startPosition = thiz->offset + skipped;

    RETURN_VAL_IF_FAIL(thiz, false);
    RETURN_VAL_IF_FAIL(length, false);

    if (thiz->available < (skipped + length))
    {
        return false;
    }

    if (bytes != NULL)
    {
        // ...o....s^^^l.
        // ^l...o.....s^^
        if (startPosition < thiz->size)
        {
            if ((startPosition + length) <= thiz->size)
            {
                memcpy(bytes, thiz->bytes + startPosition, length);
            }
            else
            {
                uint32_t len1 = startPosition + length - thiz->size;
                uint32_t len2 = length - len1;
                memcpy(bytes, thiz->bytes + startPosition, len1);
                memcpy(bytes + len1, thiz->bytes, len2);
            }
        }

        // ....s^^^l...o.
        else
        {
            startPosition -= thiz->size;
            memcpy(bytes, thiz->bytes + startPosition, length);
        }
    }

    return true;
}

TINY_LOR
bool ByteBuffer_IsFull(ByteBuffer *thiz)
{
    return thiz->available == thiz->size;
}
