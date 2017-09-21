/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonArray.h
 *
 * @remark
 *
 */

#ifndef __BSON_ARRAY_H__
#define __BSON_ARRAY_H__

#include <tiny_base.h>
#include <TinyList.h>
#include "BsonValue.h"

TINY_BEGIN_DECLS


typedef struct _BsonArray
{
    BsonValueType   type;
    TinyList        values;
    char          * string;
} BsonArray;

TINY_LOR
TINY_API
BsonArray * BsonArray_New(BsonValueType type);

TINY_LOR
TINY_API
void BsonArray_Delete(BsonArray *thiz);

#if 0
TINY_LOR
TINY_API
TinyRet BsonArray_Encode(BsonArray *thiz, bool pretty);

TINY_LOR
TINY_API
TinyRet BsonArray_AddString(BsonArray *thiz, const char *value);

TINY_LOR
TINY_API
TinyRet BsonArray_AddInteger(BsonArray *thiz, int value);

TINY_LOR
TINY_API
TinyRet BsonArray_AddFloat(BsonArray *thiz, float value);

TINY_LOR
TINY_API
TinyRet BsonArray_AddObject(BsonArray *thiz, BsonObject *value);

TINY_LOR
TINY_API
TinyRet BsonArray_AddArray(BsonArray *thiz, BsonArray *value);

TINY_LOR
TINY_API
TinyRet BsonArray_AddValue(BsonArray *thiz, BsonValue *value);
#endif


TINY_END_DECLS

#endif /* __BSON_ARRAY_H__ */