/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ByteBuffer.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __BYTE_BUFFER_H__
#define __BYTE_BUFFER_H__

#include <tiny_typedef.h>
#include <tiny_ret.h>

TINY_BEGIN_DECLS



typedef struct _ByteBuffer
{
    uint32_t          size;
    uint32_t          offset;
    uint32_t          available;
    uint8_t         * bytes;
} ByteBuffer;


TINY_API
TINY_LOR
ByteBuffer * ByteBuffer_New(uint32_t size);

TINY_API
TINY_LOR
TinyRet ByteBuffer_Construct(ByteBuffer *thiz, uint32_t size);

TINY_API
TINY_LOR
TinyRet ByteBuffer_Dispose(ByteBuffer *thiz);

TINY_API
TINY_LOR
void ByteBuffer_Delete(ByteBuffer *thiz);

TINY_API
TINY_LOR
void ByteBuffer_Clear(ByteBuffer *thiz);

TINY_API
TINY_LOR
bool ByteBuffer_Available(ByteBuffer *thiz);

TINY_API
TINY_LOR
bool ByteBuffer_Put(ByteBuffer *thiz, uint8_t *data, uint32_t length);

TINY_API
TINY_LOR
bool ByteBuffer_Get(ByteBuffer *thiz, uint32_t skipped, uint8_t bytes[], uint32_t length);

TINY_API
TINY_LOR
bool ByteBuffer_Pick(ByteBuffer *thiz, uint32_t skipped, uint8_t bytes[], uint32_t length);


TINY_END_DECLS

#endif /* __BYTE_BUFFER_H__ */