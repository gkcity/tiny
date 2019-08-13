/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonObject.h
 *
 * @remark
 *
 */

#ifndef __JSON_OBJECT_H__
#define __JSON_OBJECT_H__

#include <tiny_base.h>
#include <tiny_lor.h>
#include "JsonDefinitions.h"

TINY_BEGIN_DECLS


TINY_LOR
TINY_API
JsonObject * JsonObject_New(void);

TINY_LOR
TINY_API
JsonObject * JsonObject_NewStringFast(_IN_ const char *string);

TINY_LOR
TINY_API
JsonObject * JsonObject_NewString(_IN_ const char *string);

TINY_LOR
TINY_API
void JsonObject_Delete(_IN_ JsonObject *thiz);

TINY_LOR
TINY_API
TinyRet JsonObject_Construct(_IN_ JsonObject *thiz);

TINY_LOR
TINY_API
void JsonObject_Dispose(_IN_ JsonObject *thiz);

TINY_LOR
TINY_API
TinyRet JsonObject_Encode(_IN_ JsonObject *thiz, _IN_ bool pretty);

TINY_LOR
TINY_API
bool JsonObject_ContainsKey(_IN_ JsonObject *thiz, _IN_ const char *key);

/**
 * Getter
 */

TINY_LOR
TINY_API
JsonValue * JsonObject_GetValue(_IN_ JsonObject *thiz, _IN_ const char *key);

TINY_LOR
TINY_API
JsonObject * JsonObject_GetObject(_IN_ JsonObject *thiz, _IN_ const char *key);

TINY_LOR
TINY_API
JsonNumber * JsonObject_GetNumber(_IN_ JsonObject *thiz, _IN_ const char *key);

TINY_LOR
TINY_API
JsonArray * JsonObject_GetArray(_IN_ JsonObject *thiz, _IN_ const char *key);

TINY_LOR
TINY_API
JsonString * JsonObject_GetString(_IN_ JsonObject *thiz, _IN_ const char *key);

TINY_LOR
TINY_API
JsonBoolean * JsonObject_GetBoolean(_IN_ JsonObject *thiz, _IN_ const char *key);

/**
 * Setter: value
 */

TINY_LOR
TINY_API
TinyRet JsonObject_PutString(_IN_ JsonObject *thiz, _IN_ const char *key, _IN_ const char *value);

TINY_LOR
TINY_API
TinyRet JsonObject_PutInteger(_IN_ JsonObject *thiz, _IN_ const char *key, _IN_ int value);

TINY_LOR
TINY_API
TinyRet JsonObject_PutFloat(_IN_ JsonObject *thiz, _IN_ const char *key, _IN_ float value);

TINY_LOR
TINY_API
TinyRet JsonObject_PutBoolean(_IN_ JsonObject *thiz, _IN_ const char *key, _IN_ bool value);

TINY_LOR
TINY_API
TinyRet JsonObject_PutNull(_IN_ JsonObject *thiz, _IN_ const char *key);

/**
 * Setter: Object
 */

TINY_LOR
TINY_API
TinyRet JsonObject_PutObject(_IN_ JsonObject *thiz, _IN_ const char *key, _IN_ JsonObject *value);

TINY_LOR
TINY_API
TinyRet JsonObject_PutArray(_IN_ JsonObject *thiz, _IN_ const char *key, _IN_ JsonArray *value);

TINY_LOR
TINY_API
TinyRet JsonObject_PutNumber(_IN_ JsonObject *thiz, _IN_ const char *key, _IN_ JsonNumber *value);

TINY_LOR
TINY_API
TinyRet JsonObject_PutValue(_IN_ JsonObject *thiz, _IN_ const char *key, _IN_ JsonValue *value);


TINY_END_DECLS

#endif /* __JSON_OBJECT_H__ */