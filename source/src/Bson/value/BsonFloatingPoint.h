/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonFloatingPoint.h
 *
 * @remark
 *
 */

#ifndef __BSON_FLOATING_POINT_H__
#define __BSON_FLOATING_POINT_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonFloatingPoint
{
    double        value;
    uint32_t      length;
} BsonFloatingPoint;

TINY_LOR
BsonFloatingPoint * BsonFloatingPoint_New(const uint8_t *value);

TINY_LOR
void BsonFloatingPoint_Delete(BsonFloatingPoint *thiz);


TINY_END_DECLS

#endif /* __BSON_FLOATING_POINT_H__ */