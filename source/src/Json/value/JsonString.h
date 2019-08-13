/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonString.h
 *
 * @remark
 *
 */

#ifndef __JSON_STRING_H__
#define __JSON_STRING_H__

#include <tiny_base.h>
#include "JsonDefinitions.h"

TINY_BEGIN_DECLS


TINY_LOR
JsonString * JsonString_New(_IN_ const char *value);

TINY_LOR
void JsonString_Delete(_IN_ JsonString *thiz);


TINY_END_DECLS

#endif /* __JSON_STRING_H__ */