/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonInt64.h
 *
 * @remark
 *
 */

#ifndef __BSON_INT64_H__
#define __BSON_INT64_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonInt64
{
    int64_t         value;
} BsonInt64;

TINY_LOR
BsonInt64 * BsonInt64_New(const uint8_t *value);

TINY_LOR
void BsonInt64_Delete(BsonInt64 *thiz);


TINY_END_DECLS

#endif /* __BSON_INT64_H__ */