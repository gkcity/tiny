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

#include <TinyMapItem.h>
#include <tiny_buffer_append.h>
#include <tiny_log.h>
#include <tiny_print_binary.h>
#include "BsonEncoder.h"

#define TAG     "BsonEncoder"

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
static void uint32_to_bytes(uint32_t value, uint8_t buf[4])
{
    buf[0] = (uint8_t) ((value & 0x000000FF));
    buf[1] = (uint8_t) ((value & 0x0000FF00) >> 8);
    buf[2] = (uint8_t) ((value & 0x00FF0000) >> 16);
    buf[3] = (uint8_t) ((value & 0xFF000000) >> 24);
}

TINY_LOR
int BsonEncoder_EncodeObject(BsonObject *thiz, uint8_t *buf, uint32_t length, uint32_t offset)
{
    uint32_t size = 0;
    uint8_t total[4] = {0, 0, 0, 0};

    uint32_to_bytes(length, total);

    size += tiny_buffer_append_bytes(buf, length, offset, total, 4);

    for (uint32_t i = 0; i < thiz->data.list.size; ++i)
    {
        TinyMapItem *item = TinyList_GetAt(&thiz->data.list, i);
        BsonValue *value = (BsonValue *) (item->value);

        size += tiny_buffer_append_byte(buf, length, offset + size, value->type);
        size += tiny_buffer_append_bytes(buf, length, offset + size, (uint8_t *)item->key, strlen(item->key) + 1);
        size += BsonEncoder_EncodeValue(value, buf, length, offset + size);
    }

    size += tiny_buffer_append_byte(buf, length, offset + size, 0x0);

    return size;
}

TINY_LOR
int BsonEncoder_EncodeArray(BsonArray *thiz, uint8_t *buf, uint32_t length, uint32_t offset)
{
    return 0;
}

TINY_LOR
int BsonEncoder_EncodeValue(BsonValue *thiz, uint8_t *buf, uint32_t length, uint32_t offset)
{
    int size = 0;
    uint8_t total[4] = {0, 0, 0, 0};

    switch (thiz->type)
    {
        case BSON_FLOAT:
            break;

        case BSON_STRING:
            uint32_to_bytes(thiz->data.string->length, total);
            size += tiny_buffer_append_bytes(buf, length, offset + size, total, 4);
            size += tiny_buffer_append_bytes(buf, length, offset + size, (uint8_t *)thiz->data.string->value, thiz->data.string->length);
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
            break;

        case BSON_TIMESTAMP:
            break;

        case BSON_INT64:
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

    return size;
}