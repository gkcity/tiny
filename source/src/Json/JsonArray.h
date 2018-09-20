/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonArray.h
 *
 * @remark
 *
 */

#ifndef __JSON_ARRAY_H__
#define __JSON_ARRAY_H__

#include <tiny_base.h>
#include "JsonDefinitions.h"

TINY_BEGIN_DECLS


TINY_LOR
TINY_API
JsonArray * JsonArray_New(void);

TINY_LOR
TINY_API
JsonArray * JsonArray_NewFrom(JsonArray *src);

TINY_LOR
TINY_API
void JsonArray_Delete(JsonArray *thiz);

TINY_LOR
TINY_API
TinyRet JsonArray_Construct(JsonArray *thiz);

TINY_LOR
TINY_API
void JsonArray_Dispose(JsonArray *thiz);

TINY_LOR
TINY_API
TinyRet JsonArray_AddString(JsonArray *thiz, const char *value);

TINY_LOR
TINY_API
TinyRet JsonArray_AddInteger(JsonArray *thiz, int value);

TINY_LOR
TINY_API
TinyRet JsonArray_AddFloat(JsonArray *thiz, float value);

TINY_LOR
TINY_API
TinyRet JsonArray_AddObject(JsonArray *thiz, JsonObject *value);

TINY_LOR
TINY_API
TinyRet JsonArray_AddArray(JsonArray *thiz, JsonArray *value);

TINY_LOR
TINY_API
TinyRet JsonArray_AddValue(JsonArray *thiz, JsonValue *value);

TINY_LOR
TINY_API
bool JsonArray_CheckValuesType(JsonArray *thiz, JsonValueType type);

TINY_LOR
TINY_API
TinyRet JsonArray_Copy(JsonArray *dst, JsonArray *src);

TINY_LOR
TINY_API
TinyRet JsonArray_Clean(JsonArray *thiz);


TINY_END_DECLS

#endif /* __JSON_ARRAY_H__ */