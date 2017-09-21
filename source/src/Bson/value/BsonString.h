/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonString.h
 *
 * @remark
 *
 */

#ifndef __BSON_STRING_H__
#define __BSON_STRING_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonString
{
    char        * value;
    uint32_t      length;
} BsonString;

TINY_LOR
BsonString * BsonString_New(const char *value);

TINY_LOR
void BsonString_Delete(BsonString *thiz);


TINY_END_DECLS

#endif /* __BSON_STRING_H__ */