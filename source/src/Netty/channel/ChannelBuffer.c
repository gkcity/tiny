/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ChannelBuffer.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include "ChannelBuffer.h"

TINY_LOR
ChannelBuffer * ChannelBuffer_New(uint32_t size)
{
    ChannelBuffer *thiz = NULL;

    do
    {
        thiz = (ChannelBuffer *)tiny_malloc(sizeof(ChannelBuffer));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(ChannelBuffer_Construct(thiz, size)))
        {
            ChannelBuffer_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void ChannelBuffer_Delete(ChannelBuffer *thiz)
{
    ChannelBuffer_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
TinyRet ChannelBuffer_Construct(ChannelBuffer *thiz, uint32_t size)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        memset(thiz, 0, sizeof(ChannelBuffer));

        if (size == 0)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        thiz->bytes = tiny_malloc(size);
        if (thiz->bytes == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        thiz->size = size;
    } while (false);

    return ret;
}

TINY_LOR
void ChannelBuffer_Dispose(ChannelBuffer *thiz)
{
    if (thiz->bytes != NULL)
    {
        tiny_free(thiz->bytes);
    }

    memset(thiz, 0, sizeof(ChannelBuffer));
}

TINY_LOR
void ChannelBuffer_Clear(ChannelBuffer *thiz)
{
    thiz->offset = 0;
    thiz->available = 0;
}