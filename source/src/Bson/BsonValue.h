/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonValue.h
 *
 * @remark
 *
 */

#ifndef __BSON_VALUE_H__
#define __BSON_VALUE_H__

#include <tiny_base.h>
#include <value/BsonBinary.h>
#include <value/BsonBoolean.h>
#include <value/BsonCstring.h>
#include <value/BsonDateTime.h>
#include <value/BsonDecimal128.h>
#include <value/BsonFloatingPoint.h>
#include <value/BsonInt32.h>
#include <value/BsonInt64.h>
#include <value/BsonJavascriptCode.h>
#include <value/BsonJavascriptCodeScope.h>
#include <value/BsonMaxKey.h>
#include <value/BsonMinKey.h>
#include <value/BsonString.h>
#include <value/BsonTimestamp.h>


TINY_BEGIN_DECLS


struct _BsonObject;
typedef struct _BsonObject BsonObject;

struct _BsonArray;
typedef struct _BsonArray BsonArray;

typedef enum _BsonValueType
{
    BSON_UNDEFINED                  = 0x00,
    BSON_FLOAT                      = 0x01, /* 64-bit binary floating point */
    BSON_STRING                     = 0x02, /* UTF-8 string */
    BSON_OBJECT                     = 0x03, /* Embedded document */
    BSON_ARRAY                      = 0x04, /* Array */
    BSON_BINARY                     = 0x05, /* Binary data */
    BSON_REVERSED                   = 0x06, /* Undefined (value) — Deprecated */
    BSON_OBJECT_ID                  = 0x07, /* ObjectId (byte*12) */
    BSON_BOOLEAN                    = 0x08, /* 0x00 = false, 0x01 = true */
    BSON_DATETIME                   = 0x09, /* UTC datetime, int64 */
    BSON_NULL                       = 0x0A, /* Null value */
    BSON_CSTRING                    = 0x0B, /* Regular expression */
    BSON_DBPOINTER                  = 0x0C, /* DBPointer — Deprecated */
    BSON_JAVASCRIPT_CODE            = 0x0D, /* JavaScript Code */
    BSON_SYMBOL                     = 0x0E, /* Symbol. Deprecated */
    BSON_JAVASCRIPT_CODE_SCOPE      = 0x0F, /* JavaScript code w/ scope */
    BSON_INT32                      = 0x10, /* 32-bit integer */
    BSON_TIMESTAMP                  = 0x11, /* Timestamp, 64-bit */
    BSON_INT64                      = 0x12, /* 64-bit integer */
    BSON_DECIMAL128                 = 0x13, /* 128-bit decimal floating point */
    BSON_MIN_KEY                    = 0xFF, /* Min Key */
    BSON_MAX_KEY                    = 0x7F, /* Max Key */
} BsonValueType;

typedef union _BsonValueData
{
    BsonObject                    * object;
    BsonArray                     * array;
    BsonBinary                    * binary;
    BsonBoolean                   * boolean;
    BsonDateTime                  * datetime;
    BsonFloatingPoint             * floatingPoint;
    BsonInt32                     * int32;
    BsonInt64                     * int64;
    BsonString                    * string;
    BsonTimestamp                 * timestamp;
} BsonValueData;

typedef struct _BsonValue
{
    BsonValueType   type;
    BsonValueData   data;
} BsonValue;

TINY_LOR
TINY_API
BsonValue * BsonValue_New(void);

//TINY_LOR
//TINY_API
//BsonValue * BsonValue_NewString(const char *value);
//
//TINY_LOR
//TINY_API
//BsonValue * BsonValue_NewInteger(int value);
//
//TINY_LOR
//TINY_API
//BsonValue * BsonValue_NewFloat(float value);
//
//TINY_LOR
//TINY_API
//BsonValue * BsonValue_NewBoolean(bool value);
//
//TINY_LOR
//TINY_API
//BsonValue * BsonValue_NewNull(void);

TINY_LOR
BsonValue * BsonValue_NewValue(BsonValueType type, void *value);

TINY_LOR
TINY_API
void BsonValue_Delete(BsonValue *thiz);


TINY_END_DECLS

#endif /* __BSON_VALUE_H__ */