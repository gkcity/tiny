/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonValue.h
 *
 * @remark
 *
 */

#ifndef __JSON_VALUE_H__
#define __JSON_VALUE_H__

#include <tiny_base.h>
#include "JsonDefinitions.h"

TINY_BEGIN_DECLS



TINY_LOR
TINY_API
JsonValue * JsonValue_New(void);

TINY_LOR
TINY_API
JsonValue * JsonValue_NewString(const char *value);

TINY_LOR
TINY_API
JsonValue * JsonValue_NewInteger(long value);

TINY_LOR
TINY_API
JsonValue * JsonValue_NewFloat(float value);

TINY_LOR
TINY_API
JsonValue * JsonValue_NewBoolean(bool value);

TINY_LOR
TINY_API
JsonValue * JsonValue_NewNull(void);

TINY_LOR
TINY_API
JsonValue * JsonValue_NewValue(JsonValueType type, void *value);

TINY_LOR
TINY_API
JsonValue * JsonValue_Copy(JsonValue *other);

TINY_LOR
TINY_API
void JsonValue_Delete(JsonValue *thiz);


TINY_END_DECLS

#endif /* __JSON_VALUE_H__ */