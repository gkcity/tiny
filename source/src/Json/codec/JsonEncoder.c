/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonEncoder.h
 *
 * @remark
 *
 */

#include <TinyMapItem.h>
#include <tiny_snprintf.h>
#include <value/JsonBoolean.h>
#include <value/JsonNumber.h>
#include <value/JsonString.h>
#include <JsonArray.h>
#include "JsonEncoder.h"

#define TAG     "JsonEncoder"

TINY_LOR
static void _EncodeObject(JsonEncoder *thiz, JsonObject *object, bool pretty, int depth);

TINY_LOR
static void _EncodeArray(JsonEncoder *thiz, JsonArray *array, bool pretty, int depth);

TINY_LOR
static void _EncodeNumber(JsonEncoder *thiz, JsonNumber *number);

TINY_LOR
static void _EncodeValue(JsonEncoder *thiz, JsonValue *value, bool pretty, int depth);

TINY_LOR
static void _Append(JsonEncoder *thiz, const char *string);

TINY_LOR
static void _EncodeObject(JsonEncoder *thiz, JsonObject *object, bool pretty, int depth)
{
    RETURN_IF_FAIL(thiz);

    _Append(thiz, "{");

    if (pretty)
    {
        _Append(thiz, "\n");
    }

    for (uint32_t i = 0; i < object->data.list.size; ++i)
    {
        TinyMapItem *item = TinyList_GetAt(&object->data.list, i);

        if (pretty)
        {
            for (int d = 0; d <= depth; ++d)
            {
                _Append(thiz, "    ");
            }
        }

        _Append(thiz, "\"");
        _Append(thiz, item->key);
        _Append(thiz, "\":");

        if (pretty)
        {
            _Append(thiz, " ");
        }

        _EncodeValue(thiz, item->value, pretty, depth + 1);

        if (i < object->data.list.size - 1)
        {
            _Append(thiz, ",");
        }

        if (pretty)
        {
            _Append(thiz, "\n");
        }
    }

    if (pretty)
    {
        for (int d = 0; d < depth; ++d)
        {
            _Append(thiz, "    ");
        }
    }

    _Append(thiz, "}");
}

TINY_LOR
static void _EncodeArray(JsonEncoder *thiz, JsonArray *array, bool pretty, int depth)
{
    _Append(thiz, "[");

    for (uint32_t i = 0; i < array->values.size; ++i)
    {
        _EncodeValue(thiz, (JsonValue *) TinyList_GetAt(&array->values, i), pretty, depth);
        if (i < array->values.size - 1)
        {
            _Append(thiz, ",");
        }
    }

    _Append(thiz, "]");
}


TINY_LOR
static void _EncodeNumber(JsonEncoder *thiz, JsonNumber *number)
{
    char string[32];

    memset(string, 0, 32);

    switch (number->type)
    {
        case JSON_NUMBER_INTEGER:
            tiny_snprintf(string, 32, "%d", number->value.intValue);
            break;

        case JSON_NUMBER_FLOAT:
            tiny_snprintf(string, 32, "%f", number->value.floatValue);
            break;
    }

    _Append(thiz, string);
}

TINY_LOR
static void _EncodeValue(JsonEncoder *thiz, JsonValue *value, bool pretty, int depth)
{
    switch (value->type)
    {
        case JSON_STRING:
            _Append(thiz, "\"");
            _Append(thiz, value->data.string->value);
            _Append(thiz, "\"");
            break;

        case JSON_NUMBER:
            _EncodeNumber(thiz, value->data.number);
            break;

        case JSON_OBJECT:
            _EncodeObject(thiz, value->data.object, pretty, depth);
            break;

        case JSON_ARRAY:
            _EncodeArray(thiz, value->data.array, pretty, depth);
            break;

        case JSON_BOOLEAN:
            _Append(thiz, value->data.boolean->value ? "true" : "false");
            break;

        case JSON_NULL:
            _Append(thiz, "null");
            break;

        default:
            break;
    }
}

TINY_LOR
static void _Append(JsonEncoder *thiz, const char *string)
{
    if (thiz->buffer == NULL)
    {
        thiz->output(string, thiz->ctx);
    }
    else
    {
        uint32_t length = (uint32_t) strlen(string);
        uint32_t offset = 0;

        while (length > 0)
        {
            uint32_t size = TinyBuffer_Add(thiz->buffer, (uint8_t *)string, offset, length);
            offset += size;
            length -= size;

            if (TinyBuffer_IsFull(thiz->buffer))
            {
                thiz->out += thiz->buffer->used;
                thiz->output((const char *)thiz->buffer->bytes, thiz->ctx);

                TinyBuffer_Clear(thiz->buffer);
            }
            else
            {
                if (thiz->size == thiz->out + thiz->buffer->used)
                {
                    thiz->out += thiz->buffer->used;
                    thiz->output((const char *)thiz->buffer->bytes, thiz->ctx);
                }
            }
        }
    }
}

TINY_LOR
static void _Count (const char *string, void *ctx)
{
    JsonEncoder *thiz = (JsonEncoder *)ctx;
    thiz->size += strlen(string);
}

TINY_LOR
TinyRet JsonEncoder_Construct(JsonEncoder *thiz, JsonObject *object, bool pretty)
{
    memset(thiz, 0, sizeof(JsonEncoder));

    thiz->object = object;
    thiz->pretty = pretty;
    thiz->buffer = NULL;

    thiz->output = _Count;
    thiz->ctx = thiz;
    thiz->size = 0;
    thiz->out = 0;

    _EncodeObject(thiz, object, pretty, 0);
    thiz->output = NULL;
    thiz->ctx = NULL;

    return TINY_RET_OK;
}

TINY_LOR
void JsonEncoder_Dispose(JsonEncoder *thiz)
{
}

TINY_LOR
void JsonEncoder_EncodeObject(JsonEncoder *thiz, TinyBuffer *buffer, JsonOutput output, void *ctx)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(output);

    thiz->buffer = buffer;
    thiz->output = output;
    thiz->ctx = ctx;

    TinyBuffer_Clear(thiz->buffer);
    _EncodeObject(thiz, thiz->object, thiz->pretty, 0);
}