/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyBuffer.h
 *
 * @remark
 *
 */

#ifndef __TINY_BUFFER_H__
#define __TINY_BUFFER_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _TinyBuffer
{
    uint8_t             * bytes;
    uint32_t              size;
    uint32_t              used;
} TinyBuffer;

TINY_API
TINY_LOR
TinyBuffer * TinyBuffer_New(uint32_t size);

TINY_API
TINY_LOR
void TinyBuffer_Delete(TinyBuffer *thiz);

TINY_API
TINY_LOR
TinyRet TinyBuffer_Construct(TinyBuffer *thiz, uint32_t size);

TINY_API
TINY_LOR
void TinyBuffer_Dispose(TinyBuffer *thiz);

TINY_API
TINY_LOR
uint32_t TinyBuffer_Add(TinyBuffer *thiz, const uint8_t *data, uint32_t offset, uint32_t size);

TINY_API
TINY_LOR
void TinyBuffer_Clear(TinyBuffer *thiz);

#define TinyBuffer_IsFull(thiz) ((thiz)->size == (thiz)->used)


TINY_END_DECLS

#endif /* __TINY_BUFFER_H__ */