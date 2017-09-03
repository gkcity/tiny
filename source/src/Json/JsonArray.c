/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonArray.h
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <tiny_buffer_append.h>
#include "JsonArray.h"
#include "JsonObject.h"

#define TAG     "JsonArray"

TINY_LOR
static void _OnJsonValueDelete (void * data, void *ctx)
{
    JsonValue_Delete((JsonValue *)data);
}

TINY_LOR
static TinyRet JsonArray_Construct(JsonArray *thiz, JsonValueType type)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->type = type;

    ret = TinyList_Construct(&thiz->values);
    if (RET_SUCCEEDED(ret))
    {
        TinyList_SetDeleteListener(&thiz->values, _OnJsonValueDelete, thiz);
    }

    return ret;
}

TINY_LOR
static void JsonArray_Dispose(JsonArray *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyList_Dispose(&thiz->values);
}

TINY_LOR
JsonArray * JsonArray_New(JsonValueType type)
{
    JsonArray * thiz = NULL;

    do
    {
        thiz = (JsonArray *) tiny_malloc(sizeof(JsonArray));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(JsonArray_Construct(thiz, type)))
        {
            LOG_E(TAG, "JsonArray_Construct FAILED");
            JsonArray_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TINY_LOR
void JsonArray_Delete(JsonArray *thiz)
{
    JsonArray_Dispose(thiz);
    tiny_free(thiz);
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
TinyRet JsonArray_AddString(JsonArray *thiz, const char *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->type != JSON_STRING)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        JsonValue * v = JsonValue_NewString(value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewString FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, v);
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonArray_AddInteger(JsonArray *thiz, int value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->type != JSON_NUMBER)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        JsonValue * v = JsonValue_NewInteger(value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewInteger FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, v);
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonArray_AddFloat(JsonArray *thiz, float value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->type != JSON_NUMBER)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        JsonValue * v = JsonValue_NewFloat(value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewInteger FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, v);
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonArray_AddObject(JsonArray *thiz, JsonObject *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->type != JSON_OBJECT)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        JsonValue * v = JsonValue_NewObject(value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewInteger FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, v);
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonArray_AddArray(JsonArray *thiz, JsonArray *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->type != JSON_ARRAY)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        JsonValue * v = JsonValue_NewArray(value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewInteger FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, v);
    } while (false);

    return ret;
}