/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonDecoder.h
 *
 * @remark
 *
 */

#include <tiny_buffer_append.h>
#include <TinyMapItem.h>
#include <tiny_snprintf.h>
#include <JsonBoolean.h>
#include "JsonEncoder.h"


TINY_LOR
int JsonNumber_ToString(JsonNumber *thiz, char *buf, uint32_t length, uint32_t offset)
{
    char string[32];

    switch (thiz->type)
    {
        case JSON_NUMBER_INTEGER:
            tiny_snprintf(string, 32, "%d", thiz->value.intValue);
            break;

        case JSON_NUMBER_FLOAT:
            tiny_snprintf(string, 32, "%f", thiz->value.floatValue);
            break;
    }

    return tiny_buffer_append(buf, length, offset, string);
}

TINY_LOR
int JsonValue_ToString(JsonValue *thiz, bool pretty, int depth, char *buf, uint32_t length, uint32_t offset)
{
    int size = 0;
    char string[128];

    switch (thiz->type)
    {
        case JSON_STRING:
            tiny_snprintf(string, 128, "\"%s\"", thiz->data.string->value);
            size = tiny_buffer_append(buf, length, offset, string);
            break;

        case JSON_NUMBER:
            size = JsonNumber_ToString(thiz->data.number, buf, length, offset);
            break;

        case JSON_OBJECT:
            size = JsonObject_ToString(thiz->data.object, pretty, depth, buf, length, offset);
            break;

        case JSON_ARRAY:
            size = JsonArray_ToString(thiz->data.array, pretty, depth, buf, length, offset);
            break;

        case JSON_BOOLEAN:
            size = tiny_buffer_append(buf, length, offset, thiz->data.boolean->value ? "true" : "false");
            break;

        case JSON_NULL:
            size = tiny_buffer_append(buf, length, offset, "null");
            break;

        default:
            break;
    }

    return size;
}

TINY_LOR
int JsonArray_ToString(JsonArray *thiz, bool pretty, int depth, char *buf, uint32_t length, uint32_t offset)
{
    int size = 0;

    size += tiny_buffer_append(buf, length, offset + size, "[");

    for (uint32_t i = 0; i < thiz->values.size; ++i)
    {
        size += JsonValue_ToString((JsonValue *)TinyList_GetAt(&thiz->values, i), pretty, depth, buf, length, offset + size);

        if (i < thiz->values.size - 1)
        {
            size += tiny_buffer_append(buf, length, offset + size, ",");
        }
    }

    size += tiny_buffer_append(buf, length, offset + size, "]");

    return size;
}

TINY_LOR
int JsonObject_ToString(JsonObject *thiz, bool pretty, int depth, char *buf, uint32_t length, uint32_t offset)
{
    int size = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    size += tiny_buffer_append(buf, length, offset + size, "{");

    if (pretty)
    {
        size += tiny_buffer_append(buf, length, offset + size, "\n");
    }

    for (uint32_t i = 0; i < thiz->data.list.size; ++i)
    {
        TinyMapItem *item = TinyList_GetAt(&thiz->data.list, i);
        char key[64];

        if (pretty)
        {
            for (int d = 0; d <= depth; ++d)
            {
                size += tiny_buffer_append(buf, length, offset + size, "    ");
            }
        }

        tiny_snprintf(key, 64, "\"%s\":", item->key);
        size += tiny_buffer_append(buf, length, offset + size, key);

        if (pretty)
        {
            size += tiny_buffer_append(buf, length, offset + size, " ");
        }

        size += JsonValue_ToString(item->value, pretty, depth + 1, buf, length, offset + size);

        if (i < thiz->data.list.size - 1)
        {
            size += tiny_buffer_append(buf, length, offset + size, ",");
        }

        if (pretty)
        {
            size += tiny_buffer_append(buf, length, offset + size, "\n");
        }
    }

    if (pretty)
    {
        for (int d = 0; d < depth; ++d)
        {
            size += tiny_buffer_append(buf, length, offset + size, "    ");
        }
    }

    size += tiny_buffer_append(buf, length, offset + size, "}");

    return size;
}