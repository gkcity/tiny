/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   StringEncoder.h
 *
 * @remark
 *
 */

#ifndef __STRING_ENCODER_H__
#define __STRING_ENCODER_H__

#include <tiny_base.h>
#include <TinyList.h>
#include "BsonObject.h"
#include "BsonArray.h"
#include "BsonValue.h"

TINY_BEGIN_DECLS


#ifdef TINY_DEBUG

TINY_API
TINY_LOR
int StringEncoder_EncodeObject(BsonObject *thiz, bool pretty, int depth, char *buf, uint32_t length, uint32_t offset);

TINY_API
TINY_LOR
int StringEncoder_EncodeArray(BsonArray *thiz, bool pretty, int depth, char *buf, uint32_t length, uint32_t offset);

TINY_API
TINY_LOR
int StringEncoder_EncodeValue(BsonValue *thiz, bool pretty, int depth, char *buf, uint32_t length, uint32_t offset);

#endif /* TINY_DEBUG */

TINY_END_DECLS

#endif /* __STRING_ENCODER_H__ */