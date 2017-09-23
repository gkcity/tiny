/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonEncoder.h
 *
 * @remark
 *
 */

#ifndef __JSON_ENCODER_H__
#define __JSON_ENCODER_H__

#include <tiny_base.h>
#include <TinyList.h>
#include "JsonObject.h"

TINY_BEGIN_DECLS


TINY_API
TINY_LOR
int JsonEncoder_EncodeObject(JsonObject *thiz, bool pretty, int depth, char *buf, uint32_t length, uint32_t offset);

TINY_API
TINY_LOR
int JsonEncoder_EncodeArray(JsonArray *thiz, bool pretty, int depth, char *buf, uint32_t length, uint32_t offset);

TINY_API
TINY_LOR
int JsonEncoder_EncodeValue(JsonValue *thiz, bool pretty, int depth, char *buf, uint32_t length, uint32_t offset);


TINY_END_DECLS

#endif /* __JSON_ENCODER_H__ */