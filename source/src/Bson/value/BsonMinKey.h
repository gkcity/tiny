/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonMinKey.h
 *
 * @remark
 *
 */

#ifndef __BSON_MIN_KEY_H__
#define __BSON_MIN_KEY_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonMinKey
{
    char        * value;
    uint32_t      length;
} BsonMinKey;

TINY_LOR
BsonMinKey * BsonMinKey_New(const char *value);

TINY_LOR
void BsonMinKey_Delete(BsonMinKey *thiz);


TINY_END_DECLS

#endif /* __BSON_MIN_KEY_H__ */