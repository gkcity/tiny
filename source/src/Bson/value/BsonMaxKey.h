/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonMaxKey.h
 *
 * @remark
 *
 */

#ifndef __BSON_MAX_KEY_H__
#define __BSON_MAX_KEY_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonMaxKey
{
    char        * value;
    uint32_t      length;
} BsonMaxKey;

TINY_LOR
BsonMaxKey * BsonMaxKey_New(const char *value);

TINY_LOR
void BsonMaxKey_Delete(BsonMaxKey *thiz);


TINY_END_DECLS

#endif /* __BSON_MAX_KEY_H__ */