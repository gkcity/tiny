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

#ifdef TINY_DEBUG

#include <tiny_buffer_append.h>
#include <TinyMapItem.h>
#include <tiny_snprintf.h>
#include <value/BsonBoolean.h>
#include <tiny_log.h>
#include "StringEncoder.h"

#define TAG     "BsonDecoder"

TINY_LOR
int StringEncoder_EncodeValue(BsonValue *thiz, bool pretty, int depth, char *buf, uint32_t length, uint32_t offset)
{
    int size = 0;
    char string[128];

    switch (thiz->type)
    {
        case BSON_FLOAT:
            tiny_snprintf(string, 128, "\"%f\"", thiz->data.floatingPoint->value);
            size = tiny_buffer_append_string(buf, length, offset, string);
            break;

        case BSON_STRING:
            size += tiny_buffer_append_byte((uint8_t *) buf, length, offset, '"');
            size += tiny_buffer_append_string(buf, length, offset, thiz->data.string->value);
            size += tiny_buffer_append_byte((uint8_t *) buf, length, offset, '"');
            break;

        case BSON_OBJECT:
            break;

        case BSON_ARRAY:
            break;

        case BSON_BINARY:
            break;

        case BSON_BOOLEAN:
            break;

        case BSON_DATETIME:
            break;

        case BSON_INT32:
            tiny_snprintf(string, 128, "\"%d\"", thiz->data.int32->value);
            size = tiny_buffer_append_string(buf, length, offset, string);
            break;

        case BSON_TIMESTAMP:
            break;

        case BSON_INT64:
            tiny_snprintf(string, 128, "\"%ld\"", thiz->data.int64->value);
            size = tiny_buffer_append_string(buf, length, offset, string);
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
            LOG_D(TAG, "type: %x not support!", thiz->type);
            break;
    }

//    switch (thiz->type)
//    {
//        case JSON_STRING:
//            tiny_snprintf(string, 128, "\"%s\"", thiz->data.string->value);
//            size = tiny_buffer_append_string(buf, length, offset, string);
//            break;
//
//        case JSON_NUMBER:
//            size = StringEncoder_EncodeNumber(thiz->data.number, buf, length, offset);
//            break;
//
//        case JSON_OBJECT:
//            size = StringEncoder_EncodeObject(thiz->data.object, pretty, depth, buf, length, offset);
//            break;
//
//        case JSON_ARRAY:
//            size = StringEncoder_EncodeArray(thiz->data.array, pretty, depth, buf, length, offset);
//            break;
//
//        case JSON_BOOLEAN:
//            size = tiny_buffer_append_string(buf, length, offset, thiz->data.boolean->value ? "true" : "false");
//            break;
//
//        case JSON_NULL:
//            size = tiny_buffer_append_string(buf, length, offset, "null");
//            break;
//
//        default:
//            break;
//    }

    return size;
}

TINY_LOR
int StringEncoder_EncodeArray(BsonArray *thiz, bool pretty, int depth, char *buf, uint32_t length, uint32_t offset)
{
    int size = 0;

    size += tiny_buffer_append_string(buf, length, offset + size, "[");

    for (uint32_t i = 0; i < thiz->values.size; ++i)
    {
        size += StringEncoder_EncodeValue((BsonValue *) TinyList_GetAt(&thiz->values, i), pretty, depth, buf, length,
                                        offset + size);

        if (i < thiz->values.size - 1)
        {
            size += tiny_buffer_append_string(buf, length, offset + size, ",");
        }
    }

    size += tiny_buffer_append_string(buf, length, offset + size, "]");

    return size;
}

TINY_LOR
int StringEncoder_EncodeObject(BsonObject *thiz, bool pretty, int depth, char *buf, uint32_t length, uint32_t offset)
{
    int size = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    size += tiny_buffer_append_string(buf, length, offset + size, "{");

    if (pretty)
    {
        size += tiny_buffer_append_string(buf, length, offset + size, "\n");
    }

    for (uint32_t i = 0; i < thiz->data.list.size; ++i)
    {
        TinyMapItem *item = TinyList_GetAt(&thiz->data.list, i);
        char key[64];

        if (pretty)
        {
            for (int d = 0; d <= depth; ++d)
            {
                size += tiny_buffer_append_string(buf, length, offset + size, "    ");
            }
        }

        tiny_snprintf(key, 64, "\"%s\":", item->key);
        size += tiny_buffer_append_string(buf, length, offset + size, key);

        if (pretty)
        {
            size += tiny_buffer_append_string(buf, length, offset + size, " ");
        }

        size += StringEncoder_EncodeValue(item->value, pretty, depth + 1, buf, length, offset + size);

        if (i < thiz->data.list.size - 1)
        {
            size += tiny_buffer_append_string(buf, length, offset + size, ",");
        }

        if (pretty)
        {
            size += tiny_buffer_append_string(buf, length, offset + size, "\n");
        }
    }

    if (pretty)
    {
        for (int d = 0; d < depth; ++d)
        {
            size += tiny_buffer_append_string(buf, length, offset + size, "    ");
        }
    }

    size += tiny_buffer_append_string(buf, length, offset + size, "}");

    return size;
}

#endif /* TINY_DEBUG */