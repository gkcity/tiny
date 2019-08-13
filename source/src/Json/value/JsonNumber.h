/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonNumber.h
 *
 * @remark
 *
 */

#ifndef __JSON_NUMBER_H__
#define __JSON_NUMBER_H__

#include <tiny_base.h>
#include "JsonDefinitions.h"

TINY_BEGIN_DECLS


TINY_LOR
JsonNumber * JsonNumber_NewInteger(_IN_ long value);

TINY_LOR
JsonNumber * JsonNumber_NewFloat(_IN_ float value);

TINY_LOR
void JsonNumber_Delete(_IN_ JsonNumber *thiz);


TINY_END_DECLS

#endif /* __JSON_NUMBER_H__ */