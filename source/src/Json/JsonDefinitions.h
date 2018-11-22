/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonDefinitions.h
 *
 * @remark
 *
 */

#ifndef __JSON_DEFINITIONS_H__
#define __JSON_DEFINITIONS_H__

#include <tiny_base.h>
#include <TinyList.h>
#include <TinyMap.h>

TINY_BEGIN_DECLS


typedef struct _JsonBoolean
{
    bool        value;
} JsonBoolean;

typedef struct _JsonString
{
    char        * value;
    uint32_t      length;
} JsonString;

typedef enum _JsonNumberType
{
    JSON_NUMBER_INTEGER  = 0,
    JSON_NUMBER_FLOAT    = 1,
} JsonNumberType;

typedef union _JsonNumberValue
{
    long    intValue;
    float   floatValue;
} JsonNumberValue;

typedef struct _JsonNumber
{
    JsonNumberType      type;
    JsonNumberValue     value;
} JsonNumber;

typedef struct _JsonArray
{
    TinyList        values;
    char          * string;
} JsonArray;

typedef struct _JsonObject
{
    TinyMap         data;
    char          * string;
    uint32_t        size;
} JsonObject;

typedef enum _JsonValueType
{
    JSON_UNDEFINED      = 0,
    JSON_STRING         = 1,
    JSON_NUMBER         = 2,
    JSON_OBJECT         = 3,
    JSON_ARRAY          = 4,
    JSON_BOOLEAN        = 5,
    JSON_NULL           = 6,
} JsonValueType;

typedef union _JsonValueData
{
    JsonString    * string;
    JsonNumber    * number;
    JsonObject    * object;
    JsonArray     * array;
    JsonBoolean   * boolean;
} JsonValueData;

typedef struct _JsonValue
{
    JsonValueType   type;
    JsonValueData   data;
} JsonValue;


TINY_LOR
TINY_API
const char * JsonValueType_ToString(JsonValueType type);


TINY_END_DECLS

#endif /* __JSON_DEFINITIONS_H__ */