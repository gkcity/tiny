/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonNumber.h
 *
 * @remark
 *
 */

#ifndef __BSON_NUMBER_H__
#define __BSON_NUMBER_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef enum _BsonNumberType
{
    BSON_NUMBER_INTEGER  = 0,
    BSON_NUMBER_FLOAT    = 1,
} BsonNumberType;

typedef union _BsonNumberValue
{
    int     intValue;
    float   floatValue;
} BsonNumberValue;

typedef struct _BsonNumber
{
    BsonNumberType      type;
    BsonNumberValue     value;
} BsonNumber;

TINY_LOR
BsonNumber * BsonNumber_NewInteger(int value);

TINY_LOR
BsonNumber * BsonNumber_NewFloat(float value);

TINY_LOR
void BsonNumber_Delete(BsonNumber *thiz);


TINY_END_DECLS

#endif /* __BSON_NUMBER_H__ */