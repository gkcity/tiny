/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonValue.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "JsonValue.h"
#include "value/JsonString.h"
#include "value/JsonNumber.h"
#include "JsonObject.h"
#include "JsonArray.h"
#include "value/JsonBoolean.h"

#define TAG     "JsonValue"

TINY_LOR
static TinyRet JsonValue_Construct(JsonValue *thiz)
{
    memset(thiz, 0, sizeof(JsonValue));
    thiz->type = JSON_UNDEFINED;

    return TINY_RET_OK;
}

TINY_LOR
JsonValue * JsonValue_New(void)
{
    JsonValue * thiz = NULL;

    do
    {
        thiz = (JsonValue *) tiny_malloc(sizeof(JsonValue));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(JsonValue_Construct(thiz)))
        {
            LOG_E(TAG, "JsonValue_Construct FAILED");
            JsonValue_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TINY_LOR
JsonValue * JsonValue_NewString(const char *value)
{
    JsonValue * thiz = JsonValue_New();
    if (thiz != NULL)
    {
        thiz->type = JSON_STRING;
        thiz->data.string = JsonString_New(value);
        if (thiz->data.string == NULL)
        {
            JsonValue_Delete(thiz);
            thiz = NULL;
        }
    }

    return thiz;
}

TINY_LOR
JsonValue * JsonValue_NewInteger(int value)
{
    JsonValue * thiz = JsonValue_New();
    if (thiz != NULL)
    {
        thiz->type = JSON_NUMBER;
        thiz->data.number = JsonNumber_NewInteger(value);
        if (thiz->data.number == NULL)
        {
            JsonValue_Delete(thiz);
            thiz = NULL;
        }
    }

    return thiz;
}

TINY_LOR
JsonValue * JsonValue_NewFloat(float value)
{
    JsonValue * thiz = JsonValue_New();
    if (thiz != NULL)
    {
        thiz->type = JSON_NUMBER;
        thiz->data.number = JsonNumber_NewFloat(value);
        if (thiz->data.number == NULL)
        {
            JsonValue_Delete(thiz);
            thiz = NULL;
        }
    }

    return thiz;
}

TINY_LOR
JsonValue * JsonValue_NewBoolean(bool value)
{
    JsonValue * thiz = JsonValue_New();
    if (thiz != NULL)
    {
        thiz->type = JSON_BOOLEAN;
        thiz->data.boolean = JsonBoolean_New(value);
        if (thiz->data.boolean == NULL)
        {
            JsonValue_Delete(thiz);
            thiz = NULL;
        }
    }

    return thiz;
}

TINY_LOR
JsonValue * JsonValue_NewNull()
{
    JsonValue * thiz = JsonValue_New();
    if (thiz != NULL)
    {
        thiz->type = JSON_NULL;
    }

    return thiz;
}

TINY_LOR
JsonValue * JsonValue_NewValue(JsonValueType type, void *value)
{
    JsonValue * thiz = NULL;

    if (value != NULL)
    {
        thiz = JsonValue_New();
        if (thiz != NULL)
        {
            thiz->type = type;

            switch (type)
            {
                case JSON_STRING:
                    thiz->data.string = (JsonString *)value;
                    break;

                case JSON_NUMBER:
                    thiz->data.number = (JsonNumber *)value;
                    break;

                case JSON_OBJECT:
                    thiz->data.object = (JsonObject *)value;
                    break;

                case JSON_ARRAY:
                    thiz->data.array = (JsonArray *)value;
                    break;

                case JSON_BOOLEAN:
                    thiz->data.boolean = (JsonBoolean *)value;
                    break;

                case JSON_NULL:
                case JSON_UNDEFINED:
                    JsonValue_Delete(thiz);
                    thiz = NULL;
                    break;
            }
        }
    }

    return thiz;
}

TINY_LOR
void JsonValue_Delete(JsonValue *thiz)
{
    switch (thiz->type)
    {
        case JSON_STRING:
            if (thiz->data.string != NULL)
            {
                JsonString_Delete(thiz->data.string);
            }
            break;

        case JSON_NUMBER:
            if (thiz->data.number != NULL)
            {
                JsonNumber_Delete(thiz->data.number);
            }
            break;

        case JSON_OBJECT:
            if (thiz->data.object != NULL)
            {
                JsonObject_Delete(thiz->data.object);
            }
            break;

        case JSON_ARRAY:
            if (thiz->data.array != NULL)
            {
                JsonArray_Delete(thiz->data.array);
            }
            break;

        case JSON_BOOLEAN:
            if (thiz->data.boolean != NULL)
            {
                JsonBoolean_Delete(thiz->data.boolean);
            }
            break;

        case JSON_UNDEFINED:
        case JSON_NULL:break;
    }

    tiny_free(thiz);
}