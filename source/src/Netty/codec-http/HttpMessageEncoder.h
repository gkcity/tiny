/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpMessageEncoder.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#ifndef __HTTP_MESSAGE_ENCODER_H__
#define __HTTP_MESSAGE_ENCODER_H__

#include <tiny_base.h>
#include <tiny_api.h>
#include "../../ByteBuffer/ByteBuffer.h"
#include "HttpHeader.h"
#include "HttpContent.h"
#include "HttpMessage.h"

TINY_BEGIN_DECLS


typedef void (*HttpMessageOutput) (const uint8_t *data, uint32_t size, void *ctx);


typedef struct _HttpMessageEncoder
{
    HttpMessage         * message;
    ByteBuffer          * buffer;
    HttpMessageOutput     output;
    void                * ctx;
    uint32_t              size;
    uint32_t              out;
} HttpMessageEncoder;

TINY_API
TINY_LOR
TinyRet HttpMessageEncoder_Construct(HttpMessageEncoder *thiz, HttpMessage *message);

TINY_API
TINY_LOR
void HttpMessageEncoder_Dispose(HttpMessageEncoder *thiz);

TINY_API
TINY_LOR
void HttpMessageEncoder_Encode(HttpMessageEncoder *thiz, ByteBuffer *buffer, HttpMessageOutput output, void *ctx);



TINY_END_DECLS

#endif /* __HTTP_MESSAGE_ENCODER_H__ */