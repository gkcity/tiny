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
#include <codec/JsonEncoder.h>
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

    do 
    {
        memset(thiz, 0, sizeof(JsonArray));
        thiz->type = type;
        thiz->string = NULL;

        ret = TinyList_Construct(&thiz->values);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyList_SetDeleteListener(&thiz->values, _OnJsonValueDelete, thiz);
    } while (false);

    return ret;
}

TINY_LOR
static void JsonArray_Dispose(JsonArray *thiz)
{
    RETURN_IF_FAIL(thiz);

    if (thiz->string != NULL)
    {
        tiny_free(thiz->string);
        thiz->string = NULL;
    }

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
TinyRet JsonArray_Encode(JsonArray *thiz, bool pretty)
{
    int length = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    length = JsonEncoder_EncodeArray(thiz, pretty, 0, NULL, 0, 0);
    if (length < 0)
    {
        return TINY_RET_E_INTERNAL;
    }

    if (thiz->string != NULL)
    {
        tiny_free(thiz->string);
    }

    thiz->string = tiny_malloc((size_t)length + 1);
    if (thiz->string == NULL)
    {
        return TINY_RET_E_NEW;
    }

    memset(thiz->string, 0, (size_t)length + 1);

    JsonEncoder_EncodeArray(thiz, pretty, 0, thiz->string, (uint32_t) length, 0);

    return TINY_RET_OK;
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
        if (RET_FAILED(ret))
        {
            JsonValue_Delete(v);
        }
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
        if (RET_FAILED(ret))
        {
            JsonValue_Delete(v);
        }
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
        if (RET_FAILED(ret))
        {
            JsonValue_Delete(v);
        }
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

        JsonValue * v = JsonValue_NewValue(JSON_OBJECT, value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewInteger FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, v);
        if (RET_FAILED(ret))
        {
            JsonValue_Delete(v);
        }
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

        JsonValue * v = JsonValue_NewValue(JSON_ARRAY, value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewValue FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, v);
        if (RET_FAILED(ret))
        {
            JsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonArray_AddValue(JsonArray *thiz, JsonValue *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->type != value->type)
        {
            LOG_E(TAG, "JsonArray_AddValue FAILED, type invalid: %d", value->type);
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, value);
    } while (false);

    return ret;
}