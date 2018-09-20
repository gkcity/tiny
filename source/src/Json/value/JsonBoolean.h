/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonBoolean.h
 *
 * @remark
 *
 */

#ifndef __JSON_BOOLEAN_H__
#define __JSON_BOOLEAN_H__

#include <tiny_base.h>
#include "JsonDefinitions.h"

TINY_BEGIN_DECLS


TINY_LOR
JsonBoolean * JsonBoolean_New(bool value);

TINY_LOR
void JsonBoolean_Delete(JsonBoolean *thiz);


TINY_END_DECLS

#endif /* __JSON_BOOLEAN_H__ */