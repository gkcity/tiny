/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonInt32.h
 *
 * @remark
 *
 */

#ifndef __BSON_INT32_H__
#define __BSON_INT32_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonInt32
{
    int32_t         value;
} BsonInt32;

TINY_LOR
BsonInt32 * BsonInt32_New(const uint8_t *value);

TINY_LOR
void BsonInt32_Delete(BsonInt32 *thiz);


TINY_END_DECLS

#endif /* __BSON_INT32_H__ */