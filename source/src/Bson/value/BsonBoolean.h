/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonBoolean.h
 *
 * @remark
 *
 */

#ifndef __BSON_BOOLEAN_H__
#define __BSON_BOOLEAN_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonBoolean
{
    bool            value;
} BsonBoolean;

TINY_LOR
BsonBoolean * BsonBoolean_New(uint8_t value);

TINY_LOR
void BsonBoolean_Delete(BsonBoolean *thiz);


TINY_END_DECLS

#endif /* __BSON_BOOLEAN_H__ */