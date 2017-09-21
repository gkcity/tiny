/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonDateTime.h
 *
 * @remark
 *
 */

#ifndef __BSON_DATETIME_H__
#define __BSON_DATETIME_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonDateTime
{
    int64_t       value;
} BsonDateTime;

TINY_LOR
BsonDateTime * BsonDateTime_New(const uint8_t *value);

TINY_LOR
void BsonDateTime_Delete(BsonDateTime *thiz);


TINY_END_DECLS

#endif /* __BSON_DATETIME_H__ */