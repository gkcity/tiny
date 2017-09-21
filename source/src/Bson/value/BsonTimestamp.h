/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonTimestamp.h
 *
 * @remark
 *
 */

#ifndef __BSON_TIMESTAMP_H__
#define __BSON_TIMESTAMP_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonTimestamp
{
    int64_t         value;
} BsonTimestamp;

TINY_LOR
BsonTimestamp * BsonTimestamp_New(const uint8_t *value);

TINY_LOR
void BsonTimestamp_Delete(BsonTimestamp *thiz);


TINY_END_DECLS

#endif /* __BSON_TIMESTAMP_H__ */