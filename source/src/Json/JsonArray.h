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
JsonArray * JsonArray_NewFrom(_IN_ JsonArray *src);

TINY_LOR
TINY_API
void JsonArray_Delete(_IN_ JsonArray *thiz);

TINY_LOR
TINY_API
TinyRet JsonArray_Construct(_IN_ JsonArray *thiz);

TINY_LOR
TINY_API
void JsonArray_Dispose(_IN_ JsonArray *thiz);

TINY_LOR
TINY_API
TinyRet JsonArray_AddString(_IN_ JsonArray *thiz, _IN_ const char *value);

TINY_LOR
TINY_API
TinyRet JsonArray_AddInteger(_IN_ JsonArray *thiz, _IN_ int value);

TINY_LOR
TINY_API
TinyRet JsonArray_AddFloat(_IN_ JsonArray *thiz, _IN_ float value);

TINY_LOR
TINY_API
TinyRet JsonArray_AddObject(_IN_ JsonArray *thiz, _IN_ JsonObject *value);

TINY_LOR
TINY_API
TinyRet JsonArray_AddArray(_IN_ JsonArray *thiz, _IN_ JsonArray *value);

TINY_LOR
TINY_API
TinyRet JsonArray_AddValue(_IN_ JsonArray *thiz, _IN_ JsonValue *value);

TINY_LOR
TINY_API
bool JsonArray_CheckValuesType(_IN_ JsonArray *thiz, _IN_ JsonValueType type);

TINY_LOR
TINY_API
TinyRet JsonArray_Copy(_OU_ JsonArray *dst, _IN_ JsonArray *src);

TINY_LOR
TINY_API
TinyRet JsonArray_Clean(_IN_ JsonArray *thiz);


TINY_END_DECLS

#endif /* __JSON_ARRAY_H__ */