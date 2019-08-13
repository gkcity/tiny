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
ByteBuffer * ByteBuffer_New(_IN_ uint32_t size);

TINY_API
TINY_LOR
TinyRet ByteBuffer_Construct(_IN_ ByteBuffer *thiz, _IN_ uint32_t size);

TINY_API
TINY_LOR
TinyRet ByteBuffer_Dispose(_IN_ ByteBuffer *thiz);

TINY_API
TINY_LOR
void ByteBuffer_Delete(_IN_ ByteBuffer *thiz);

TINY_API
TINY_LOR
void ByteBuffer_Clear(_IN_ ByteBuffer *thiz);

TINY_API
TINY_LOR
bool ByteBuffer_Available(_IN_ ByteBuffer *thiz);

TINY_API
TINY_LOR
bool ByteBuffer_Put(_IN_ ByteBuffer *thiz, _IN_ uint8_t *data, _IN_ uint32_t length);

TINY_API
TINY_LOR
uint32_t ByteBuffer_Add(_IN_ ByteBuffer *thiz, _IN_ const uint8_t *data, _IN_ uint32_t offset, _IN_ uint32_t size);

TINY_API
TINY_LOR
bool ByteBuffer_Get(_IN_ ByteBuffer *thiz, _IN_ uint32_t skipped, _OU_ uint8_t bytes[], _IN_ uint32_t length);

TINY_API
TINY_LOR
bool ByteBuffer_Pick(_IN_ ByteBuffer *thiz, _IN_ uint32_t skipped, _OU_ uint8_t bytes[], _IN_ uint32_t length);

TINY_API
TINY_LOR
bool ByteBuffer_IsFull(_IN_ ByteBuffer *thiz);


TINY_END_DECLS

#endif /* __BYTE_BUFFER_H__ */