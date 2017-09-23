/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonObject.h
 *
 * @remark
 *
 */

#ifndef __BSON_OBJECT_H__
#define __BSON_OBJECT_H__

#include <tiny_base.h>
#include <tiny_lor.h>
#include <TinyMap.h>
#include "BsonValue.h"

TINY_BEGIN_DECLS


typedef struct _BsonObject
{
    TinyMap         data;
    uint8_t       * binary;
    uint32_t        size;
} BsonObject;

TINY_LOR
TINY_API
BsonObject * BsonObject_New(void);

TINY_LOR
TINY_API
BsonObject * BsonObject_NewBinary(const uint8_t *binary, uint32_t length);

TINY_LOR
TINY_API
void BsonObject_Delete(BsonObject *thiz);

TINY_LOR
TINY_API
TinyRet BsonObject_Construct(BsonObject *thiz);

TINY_LOR
TINY_API
void BsonObject_Dispose(BsonObject *thiz);

TINY_LOR
TINY_API
TinyRet BsonObject_Encode(BsonObject *thiz);

#ifdef TINY_DEBUG
TINY_LOR
TINY_API
TinyRet BsonObject_ToString(BsonObject *thiz);
#endif

TINY_LOR
TINY_API
bool BsonObject_ContainsKey(BsonObject *thiz, const char *key);

TINY_LOR
TINY_API
BsonValue * BsonObject_GetValue(BsonObject *thiz, const char *key);

#if 0
TINY_LOR
TINY_API
BsonObject * BsonObject_GetObject(BsonObject *thiz, const char *key);

TINY_LOR
TINY_API
BsonNumber * BsonObject_GetNumber(BsonObject *thiz, const char *key);

TINY_LOR
TINY_API
BsonArray * BsonObject_GetArray(BsonObject *thiz, const char *key);

TINY_LOR
TINY_API
BsonString * BsonObject_GetString(BsonObject *thiz, const char *key);
#endif

TINY_LOR
TINY_API
TinyRet BsonObject_PutString(BsonObject *thiz, const char *key, const char *value);

TINY_LOR
TINY_API
TinyRet BsonObject_PutInt32(BsonObject *thiz, const char *key, int32_t value);

TINY_LOR
TINY_API
TinyRet BsonObject_PutFloat(BsonObject *thiz, const char *key, float value);

TINY_LOR
TINY_API
TinyRet BsonObject_PutBoolean(BsonObject *thiz, const char *key, bool value);

#if 0
TINY_LOR
TINY_API
TinyRet BsonObject_PutNull(BsonObject *thiz, const char *key);

TINY_LOR
TINY_API
TinyRet BsonObject_PutObject(BsonObject *thiz, const char *key, BsonObject *value);

TINY_LOR
TINY_API
TinyRet BsonObject_PutArray(BsonObject *thiz, const char *key, BsonArray *value);

TINY_LOR
TINY_API
TinyRet BsonObject_PutNumber(BsonObject *thiz, const char *key, BsonNumber *value);
#endif

TINY_LOR
TINY_API
TinyRet BsonObject_PutValue(BsonObject *thiz, const char *key, BsonValue *value);


TINY_END_DECLS

#endif /* __BSON_OBJECT_H__ */