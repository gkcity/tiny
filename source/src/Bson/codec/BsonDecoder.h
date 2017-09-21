/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonDecoder.h
 *
 * @remark
 *
 */

#ifndef __BSON_DECODER_H__
#define __BSON_DECODER_H__

#include <tiny_base.h>
#include <TinyList.h>
#include <BsonObject.h>

TINY_BEGIN_DECLS



typedef struct _BsonDecoder
{
    const uint8_t   * value;
    uint32_t          offset;
    uint32_t          length;
} BsonDecoder;

TINY_LOR
TinyRet BsonDecoder_Construct(BsonDecoder *thiz, const uint8_t *value, uint32_t offset, uint32_t length);

TINY_LOR
void BsonDecoder_Dispose(BsonDecoder *thiz);

TINY_LOR
TinyRet BsonDecoder_Parse(BsonDecoder *thiz);

TINY_LOR
BsonObject * BsonDecoder_ConvertToObject(BsonDecoder *thiz);


TINY_END_DECLS

#endif /* __BSON_DECODER_H__ */