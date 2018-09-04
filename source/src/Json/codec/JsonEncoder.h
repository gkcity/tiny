/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonEncoder.h
 *
 * @remark
 *
 */

#ifndef __JSON_ENCODER_H__
#define __JSON_ENCODER_H__

#include <tiny_base.h>
#include <TinyList.h>
#include <ByteBuffer.h>
#include "JsonObject.h"

TINY_BEGIN_DECLS


typedef void (*JsonOutput) (const char *string, void *ctx);


typedef struct _JsonEncoder
{
    JsonOutput            output;
    void                * ctx;
    ByteBuffer          * buffer;
    JsonObject          * object;
    bool                  pretty;
    uint32_t              size;
    uint32_t              out;
} JsonEncoder;

TINY_API
TINY_LOR
TinyRet JsonEncoder_Construct(JsonEncoder *thiz, JsonObject *object, bool pretty);

TINY_API
TINY_LOR
void JsonEncoder_Dispose(JsonEncoder *thiz);

TINY_API
TINY_LOR
void JsonEncoder_EncodeObject(JsonEncoder *thiz, ByteBuffer *buffer, JsonOutput output, void *ctx);


TINY_END_DECLS

#endif /* __JSON_ENCODER_H__ */