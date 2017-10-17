/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonObject.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <TinyMapItem.h>
#include "JsonObject.h"
#include "value/JsonNumber.h"
#include "codec/JsonEncoder.h"
#include "codec/JsonDecoder.h"
#include "codec/JsonDecoderCompact.h"

#define TAG     "JsonObject"

TINY_LOR
static void _OnJsonValueDelete (void * data, void *ctx)
{
    JsonValue_Delete((JsonValue *)data);
}

TINY_LOR
JsonObject * JsonObject_New(void)
{
	JsonObject * thiz = NULL;

	do
	{
		thiz = (JsonObject *) tiny_malloc(sizeof(JsonObject));
		if (thiz == NULL)
		{
			break;
		}

        if (RET_FAILED(JsonObject_Construct(thiz)))
        {
            LOG_E(TAG, "JsonObject_Construct FAILED");
            JsonObject_Delete(thiz);
            thiz = NULL;
            break;
        }
	} while (false);

	return thiz;
}

TINY_LOR
void JsonObject_Delete(JsonObject *thiz)
{
    JsonObject_Dispose(thiz);
	tiny_free(thiz);
}

TINY_LOR
TinyRet JsonObject_Construct(JsonObject *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do 
    {
        memset(thiz, 0, sizeof(JsonObject));
        thiz->string = NULL;
        thiz->size = 0;

        ret = TinyMap_Construct(&thiz->data);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyMap_SetDeleteListener(&thiz->data, _OnJsonValueDelete, thiz);
    } while (0);

    return ret;
}

TINY_LOR
void JsonObject_Dispose(JsonObject *thiz)
{
    RETURN_IF_FAIL(thiz);

    if (thiz->string != NULL)
    {
        tiny_free(thiz->string);
        thiz->string = NULL;
    }

    TinyMap_Dispose(&thiz->data);
}

TINY_LOR
JsonObject * JsonObject_NewStringFast(const char *string)
{
    JsonObject *object = NULL;

    RETURN_VAL_IF_FAIL(string, NULL);

    do
    {
        JsonDecoder decoder;

        if (RET_FAILED(JsonDecoder_Construct(&decoder)))
        {
            break;
        }

        if (RET_SUCCEEDED(JsonDecoder_Parse(&decoder, string)))
        {
            object = JsonDecoder_ConvertToObject(&decoder);
        }

        JsonDecoder_Dispose(&decoder);
    } while (false);

    return object;
}

TINY_LOR
JsonObject * JsonObject_NewString(const char *string)
{
    JsonObject *object = NULL;

    RETURN_VAL_IF_FAIL(string, NULL);

    do
    {
        JsonDecoderCompact decoder;

        if (RET_FAILED(JsonDecoderCompact_Construct(&decoder)))
        {
            break;
        }

        object = JsonDecoderCompact_Parse(&decoder, string);

        JsonDecoderCompact_Dispose(&decoder);
    } while (false);

    return object;
}

TINY_LOR
TinyRet JsonObject_Encode(JsonObject *thiz, bool pretty)
{
    int length = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    length = JsonEncoder_EncodeObject(thiz, pretty, 0, NULL, 0, 0);
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
    thiz->size = (uint32_t)length;

    JsonEncoder_EncodeObject(thiz, pretty, 0, thiz->string, (uint32_t) length, 0);

    return TINY_RET_OK;
}

TINY_LOR
bool JsonObject_ContainsKey(JsonObject *thiz, const char *key)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);

    return (TinyMap_GetValue(&thiz->data, key) != NULL);
}

TINY_LOR
JsonValue * JsonObject_GetValue(JsonObject *thiz, const char *key)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

	return (JsonValue *) TinyMap_GetValue(&thiz->data, key);
}

TINY_LOR
JsonObject * JsonObject_GetObject(JsonObject *thiz, const char *key)
{
    JsonObject *object = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

    do
    {
        JsonValue *value = JsonObject_GetValue(thiz, key);
        if (value == NULL)
        {
            break;
        }

        if (value->type != JSON_OBJECT)
        {
            break;
        }

        object = value->data.object;
    } while (false);

    return object;
}

TINY_LOR
JsonNumber *JsonObject_GetNumber(JsonObject *thiz, const char *key)
{
    JsonNumber *number = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

    do
    {
        JsonValue *value = JsonObject_GetValue(thiz, key);
        if (value == NULL)
        {
            break;
        }

        if (value->type != JSON_NUMBER)
        {
            break;
        }

        number = value->data.number;
    } while (false);

    return number;
}

TINY_LOR
JsonArray *JsonObject_GetArray(JsonObject *thiz, const char *key)
{
    JsonArray *array = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

    do
    {
        JsonValue *value = JsonObject_GetValue(thiz, key);
        if (value == NULL)
        {
            break;
        }

        if (value->type != JSON_ARRAY)
        {
            break;
        }

        array = value->data.array;
    } while (false);

    return array;
}

TINY_LOR
JsonString *JsonObject_GetString(JsonObject *thiz, const char *key)
{
    JsonString *string = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

    do
    {
        JsonValue *value = JsonObject_GetValue(thiz, key);
        if (value == NULL)
        {
            break;
        }

        if (value->type != JSON_STRING)
        {
            break;
        }

        string = value->data.string;
    } while (false);

    return string;
}

TINY_LOR
JsonBoolean * JsonObject_GetBoolean(JsonObject *thiz, const char *key)
{
    JsonBoolean *boolean = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

    do
    {
        JsonValue *value = JsonObject_GetValue(thiz, key);
        if (value == NULL)
        {
            break;
        }

        if (value->type != JSON_BOOLEAN)
        {
            break;
        }

        boolean = value->data.boolean;
    } while (false);

    return boolean;
}

TINY_LOR
TinyRet JsonObject_PutString(JsonObject *thiz, const char *key, const char *value)
{
	TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);
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

        ret = JsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "JsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            JsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonObject_PutInteger(JsonObject *thiz, const char *key, int value)
{
	TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);

    do
    {
        JsonValue * v = JsonValue_NewInteger(value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewInteger FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = JsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "JsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            JsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonObject_PutFloat(JsonObject *thiz, const char *key, float value)
{
	TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);

    do
    {
        JsonValue * v = JsonValue_NewFloat(value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewFloat FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = JsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "JsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            JsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonObject_PutBoolean(JsonObject *thiz, const char *key, bool value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);

    do
    {
        JsonValue * v = JsonValue_NewBoolean(value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewBoolean FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = JsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "JsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            JsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonObject_PutNull(JsonObject *thiz, const char *key)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);

    do
    {
        JsonValue * v = JsonValue_NewNull();
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewNull FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = JsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "JsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            JsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonObject_PutObject(JsonObject *thiz, const char *key, JsonObject *value)
{
	TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        JsonValue * v = JsonValue_NewValue(JSON_OBJECT, value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewObject FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = JsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "JsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            JsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonObject_PutArray(JsonObject *thiz, const char *key, JsonArray *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        JsonValue * v = JsonValue_NewValue(JSON_ARRAY, value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewArray FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = JsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "JsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            JsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonObject_PutNumber(JsonObject *thiz, const char *key, JsonNumber *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        JsonValue * v = JsonValue_NewValue(JSON_NUMBER, value);
        if (v == NULL)
        {
            LOG_E(TAG, "JsonValue_NewArray FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = JsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "JsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            JsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet JsonObject_PutValue(JsonObject *thiz, const char *key, JsonValue *value)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    TinyMap_Erase(&thiz->data, key);

    return TinyMap_Insert(&thiz->data, key, value);
}