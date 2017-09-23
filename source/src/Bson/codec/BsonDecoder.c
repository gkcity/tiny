/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonDecoder.h
 *
 * @remark
 *
 */

#include <tiny_log.h>
#include <value/BsonString.h>
#include <value/BsonFloatingPoint.h>
#include <value/BsonBinary.h>
#include <value/BsonBoolean.h>
#include <value/BsonDateTime.h>
#include <value/BsonTimestamp.h>
#include <value/BsonInt32.h>
#include <value/BsonInt64.h>
#include "BsonDecoder.h"

#define TAG                 "BsonDecoder"
#define BSON_MIN_LENGTH     (4 + 1 + 2 + 4 + 2 + 1)

/**
 * BSON: {"hello": "world"} => BSON: \x16\x00\x00\x00\x02hello\x00\x06\x00\x00\x00world\x00\x00
 *
 * [\x16\x00\x00\x00]   = total length                      4
 * [\x02]               = type of value: string,            1
 * [hello\x00]          = key.                              2
 * [\x06\x00\x00\x00]   = length of value.                  4
 * [world\x00]          = value                             2
 * [\x00]               = end                               1
 *
 */

TINY_LOR
static BsonObject * BsonDecoder_GetObject(BsonDecoder *thiz);

TINY_LOR
static uint32_t BsonTokenizer_GetUint32Value(BsonDecoder *thiz)
{
    const uint8_t *p = thiz->value + thiz->offset;
    uint32_t a =
            (uint32_t)(p[0]) +
            (((uint32_t)(p[1])) << 8) +
            (((uint32_t)(p[2])) << 16) +
            (((uint32_t)(p[3])) << 24);
    thiz->offset += 4;
    return a;
}

TINY_LOR
static TinyRet BsonDecoder_GetKey(BsonDecoder *thiz, char *string, uint32_t length)
{
    TinyRet ret = TINY_RET_OK;

    LOG_D(TAG, "BsonDecoder_GetKey");

    do
    {
        uint32_t i = thiz->offset;

        while (thiz->value[i] != '\0' && i < thiz->length)
        {
            i++;
        }

        if (i == thiz->length)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        if ((i - thiz->offset) > length)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        memset(string, 0, length);
        strncpy(string, (const char *)(thiz->value + thiz->offset), (i - thiz->offset));
        thiz->offset = i + 1;
    } while (false);

    return ret;
}

TINY_LOR
static BsonString * BsonDecoder_GetString(BsonDecoder *thiz)
{
    BsonString *string = NULL;

    LOG_D(TAG, "BsonDecoder_GetString");

    do
    {
        uint32_t length = BsonTokenizer_GetUint32Value(thiz);

        string = BsonString_New((const char *)(thiz->value + thiz->offset));
        if (string == NULL)
        {
            break;
        }

        thiz->offset += length;
    } while (false);

    return string;
}

TINY_LOR
static BsonFloatingPoint * BsonDecoder_GetFloat(BsonDecoder *thiz)
{
    BsonFloatingPoint *v = NULL;

    LOG_D(TAG, "BsonDecoder_GetFloat");

    v = BsonFloatingPoint_New(thiz->value);
    if (v != NULL)
    {
        thiz->offset += 8;
    }

    return v;
}

TINY_LOR
static BsonArray * BsonDecoder_GetArray(BsonDecoder *thiz)
{
    LOG_D(TAG, "BsonDecoder_GetArray");

    return NULL;
}

TINY_LOR
static BsonBinary * BsonDecoder_GetBinary(BsonDecoder *thiz)
{
    BsonBinary *v = NULL;

    LOG_D(TAG, "BsonDecoder_GetBinary");

    do
    {
        BsonBinaryType type = (BsonBinaryType) thiz->value[thiz->offset++];
        uint32_t length = BsonTokenizer_GetUint32Value(thiz);
        if (length > (thiz->length - thiz->offset))
        {
            break;
        }

        v = BsonBinary_New(type, thiz->value + thiz->offset, length);
        if (v == NULL)
        {
            break;
        }

        thiz->offset += 8;
    } while (false);

    return v;
}

TINY_LOR
static BsonBoolean * BsonDecoder_GetBoolean(BsonDecoder *thiz)
{
    BsonBoolean *v = NULL;

    LOG_D(TAG, "BsonDecoder_GetBoolean");

    do
    {
        v = BsonBoolean_New(thiz->value[thiz->offset++]);
        if (v == NULL)
        {
            break;
        }

        thiz->offset ++;
    } while (false);

    return v;
}

TINY_LOR
static BsonDateTime * BsonDecoder_GetDateTime(BsonDecoder *thiz)
{
    BsonDateTime * v = NULL;

    LOG_D(TAG, "BsonDecoder_GetDateTime");

    do
    {
        v = BsonDateTime_New(thiz->value + thiz->offset);
        if (v == NULL)
        {
            break;
        }

        thiz->offset += 8;
    } while (false);

    return v;
}

TINY_LOR
static BsonTimestamp * BsonDecoder_GetTimestamp(BsonDecoder *thiz)
{
    BsonTimestamp * v = NULL;

    LOG_D(TAG, "BsonDecoder_GetTimestamp");

    do
    {
        v = BsonTimestamp_New(thiz->value + thiz->offset);
        if (v == NULL)
        {
            break;
        }

        thiz->offset += 8;
    } while (false);

    return v;
}

TINY_LOR
static BsonInt32 * BsonDecoder_GetInt32(BsonDecoder *thiz)
{
    BsonInt32 * v = NULL;

    LOG_D(TAG, "BsonDecoder_GetInt32");

    do
    {
        v = BsonInt32_New(thiz->value + thiz->offset);
        if (v == NULL)
        {
            break;
        }

        thiz->offset += 4;
    } while (false);

    return v;
}

TINY_LOR
static BsonInt64 * BsonDecoder_GetInt64(BsonDecoder *thiz)
{
    BsonInt64 * v = NULL;

    LOG_D(TAG, "BsonDecoder_GetInt64");

    do
    {
        v = BsonInt64_New(thiz->value + thiz->offset);
        if (v == NULL)
        {
            break;
        }

        thiz->offset += 8;
    } while (false);

    return v;
}

TINY_LOR
static BsonValue * BsonDecoder_GetValue(BsonDecoder *thiz, BsonValueType type)
{
    BsonValue * value = NULL;

    LOG_D(TAG, "BsonDecoder_GetValue: 0x%x", type);

    switch (type)
    {
        case BSON_FLOAT:
            value = BsonValue_NewValue(BSON_FLOAT, BsonDecoder_GetFloat(thiz));
            break;

        case BSON_STRING:
            value = BsonValue_NewValue(BSON_STRING, BsonDecoder_GetString(thiz));
            break;

        case BSON_OBJECT:
            value = BsonValue_NewValue(BSON_OBJECT, BsonDecoder_GetObject(thiz));
            break;

        case BSON_ARRAY:
            value = BsonValue_NewValue(BSON_ARRAY, BsonDecoder_GetArray(thiz));
            break;

        case BSON_BINARY:
            value = BsonValue_NewValue(BSON_BINARY, BsonDecoder_GetBinary(thiz));
            break;

        case BSON_BOOLEAN:
            value = BsonValue_NewValue(BSON_BOOLEAN, BsonDecoder_GetBoolean(thiz));
            break;

        case BSON_DATETIME:
            value = BsonValue_NewValue(BSON_DATETIME, BsonDecoder_GetDateTime(thiz));
            break;

        case BSON_TIMESTAMP:
            value = BsonValue_NewValue(BSON_TIMESTAMP, BsonDecoder_GetTimestamp(thiz));
            break;

        case BSON_INT32:
            value = BsonValue_NewValue(BSON_INT32, BsonDecoder_GetInt32(thiz));
            break;

        case BSON_INT64:
            value = BsonValue_NewValue(BSON_INT64, BsonDecoder_GetInt64(thiz));
            break;

        case BSON_NULL:
        case BSON_JAVASCRIPT_CODE:
        case BSON_JAVASCRIPT_CODE_SCOPE:
        case BSON_DBPOINTER:
        case BSON_CSTRING:
        case BSON_SYMBOL:
        case BSON_DECIMAL128:
        case BSON_MIN_KEY:
        case BSON_MAX_KEY:
        case BSON_REVERSED:
        case BSON_OBJECT_ID:
        case BSON_UNDEFINED:
            LOG_D(TAG, "type: %x not support!", type);
            break;
    }

    return value;
}

TINY_LOR
TinyRet BsonDecoder_Construct(BsonDecoder *thiz, const uint8_t *value, uint32_t offset, uint32_t length)
{
    LOG_D(TAG, "BsonDecoder_Construct");

    thiz->value = value;
    thiz->offset = 0;
    thiz->length = length;

    return TINY_RET_OK;
}

TINY_LOR
void BsonDecoder_Dispose(BsonDecoder *thiz)
{
    LOG_D(TAG, "BsonDecoder_Dispose");

}

TINY_LOR
TinyRet BsonDecoder_Parse(BsonDecoder *thiz)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        uint32_t length = thiz->length - thiz->offset;
        uint32_t value = 0;

        if (length < BSON_MIN_LENGTH)
        {
            LOG_D(TAG, "invalid length: %d", length);
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        value = BsonTokenizer_GetUint32Value(thiz);
        if (value != length)
        {
            LOG_D(TAG, "length(%d) not equals %d", value, length);
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }
    } while (false);

    return ret;
}

TINY_LOR
static BsonObject * BsonDecoder_GetObject(BsonDecoder *thiz)
{
    TinyRet ret = TINY_RET_OK;
    BsonObject * object = NULL;

    do
    {
        object = BsonObject_New();
        if (object == NULL)
        {
            break;
        }

        while (thiz->offset < thiz->length)
        {
            BsonValueType type = (BsonValueType) thiz->value[thiz->offset++];
            BsonValue *value = NULL;
            char key[128];

            ret = BsonDecoder_GetKey(thiz, key, 128);
            if (RET_FAILED(ret))
            {
                LOG_E(TAG, "BsonDecoder_GetKey FAILED: %d", TINY_RET_CODE(ret));
                break;
            }

            value = BsonDecoder_GetValue(thiz, type);
            if (value == NULL)
            {
                LOG_E(TAG, "BsonDecoder_GetValue FAILED: %s, type is: %x", key, type);
                ret = TINY_RET_E_ARG_INVALID;
                break;
            }

            printf("key: %s\n", key);
            printf("value: %s\n", value->data.string->value);

            BsonObject_PutValue(object, key, value);

            if (thiz->value[thiz->offset] == 0x00)
            {
                break;
            }
        }

        if (RET_FAILED(ret))
        {
            BsonObject_Delete(object);
            object = NULL;
        }
    } while (false);

    return object;
}

TINY_LOR
BsonObject * BsonDecoder_ConvertToObject(BsonDecoder *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    LOG_D(TAG, "BsonDecoder_ConvertToObject");

    return BsonDecoder_GetObject(thiz);
}