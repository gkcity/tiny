/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonEncoder.h
 *
 * @remark
 *
 */

#ifndef __BSON_ENCODER_H__
#define __BSON_ENCODER_H__

#include <tiny_base.h>
#include <BsonObject.h>
#include <BsonArray.h>
#include <BsonValue.h>

TINY_BEGIN_DECLS


TINY_API
TINY_LOR
int BsonEncoder_EncodeObject(BsonObject *thiz, uint8_t *buf, uint32_t length, uint32_t offset);

TINY_API
TINY_LOR
int BsonEncoder_EncodeArray(BsonArray *thiz, uint8_t *buf, uint32_t length, uint32_t offset);

TINY_API
TINY_LOR
int BsonEncoder_EncodeValue(BsonValue *thiz, uint8_t *buf, uint32_t length, uint32_t offset);


TINY_END_DECLS

#endif /* __BSON_ENCODER_H__ */