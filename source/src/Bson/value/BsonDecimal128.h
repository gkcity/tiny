/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonDecimal128.h
 *
 * @remark
 *
 */

#ifndef __BSON_DECIMAL128_H__
#define __BSON_DECIMAL128_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonDecimal128
{
    int64_t       value;
    uint32_t      length;
} BsonDecimal128;

TINY_LOR
BsonDecimal128 * BsonDecimal128_New(const uint8_t *value);

TINY_LOR
void BsonDecimal128_Delete(BsonDecimal128 *thiz);


TINY_END_DECLS

#endif /* __BSON_DECIMAL128_H__ */