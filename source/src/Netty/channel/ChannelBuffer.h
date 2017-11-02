/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ChannelBuffer.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __CHANNEL_BUFFER_H__
#define __CHANNEL_BUFFER_H__

#include <tiny_typedef.h>
#include <tiny_ret.h>

TINY_BEGIN_DECLS

typedef struct _ChannelBuffer
{
    uint8_t             * bytes;
    uint32_t              size;
    uint32_t              offset;
    int                   available;
} ChannelBuffer;

TINY_LOR
ChannelBuffer * ChannelBuffer_New(uint32_t size);

TINY_LOR
void ChannelBuffer_Delete(ChannelBuffer *thiz);

TINY_LOR
TinyRet ChannelBuffer_Construct(ChannelBuffer *thiz, uint32_t size);

TINY_LOR
void ChannelBuffer_Dispose(ChannelBuffer *thiz);

TINY_LOR
void ChannelBuffer_Clear(ChannelBuffer *thiz);


TINY_END_DECLS

#endif /* __CHANNEL_BUFFER_H__ */