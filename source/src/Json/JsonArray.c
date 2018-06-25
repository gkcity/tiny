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
#include <codec/JsonEncoder.h>
#include "JsonArray.h"

#define TAG     "JsonArray"

TINY_LOR
static void _OnJsonValueDelete (void * data, void *ctx)
{
    JsonValue_Delete((JsonValue *)data);
}

TINY_LOR
static TinyRet JsonArray_Construct(JsonArray *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do 
    {
        memset(thiz, 0, sizeof(JsonArray));
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
JsonArray * JsonArray_New(void)
{
    JsonArray * thiz = NULL;

    do
    {
        thiz = (JsonArray *) tiny_malloc(sizeof(JsonArray));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(JsonArray_Construct(thiz)))
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
TinyRet JsonArray_AddString(JsonArray *thiz, const char *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
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
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    return TinyList_AddTail(&thiz->values, value);
}

TINY_LOR
bool JsonArray_CheckValuesType(JsonArray *thiz, JsonValueType type)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    for (int i = 0; i < thiz->values.size; ++i)
    {
        JsonValue *v = (JsonValue *) TinyList_GetAt(&thiz->values, i);
        if (v->type != JSON_OBJECT) {
            return false;
        }
    }

    return true;
}
