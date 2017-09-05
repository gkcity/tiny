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
#include <tiny_snprintf.h>
#include <tiny_buffer_append.h>
#include "JsonObject.h"
#include "JsonNumber.h"
#include "JsonTokenizer.h"

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

    thiz->string = NULL;

    ret = TinyMap_Construct(&thiz->data);
    if (RET_SUCCEEDED(ret))
    {
        TinyMap_SetDeleteListener(&thiz->data, _OnJsonValueDelete, thiz);
    }

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
JsonObject * JsonObject_NewString(const char *string)
{
    JsonObject *object = NULL;
    JsonTokenizer tokenizer;

    RETURN_VAL_IF_FAIL(string, NULL);

    if (RET_FAILED(JsonTokenizer_Construct(&tokenizer)))
    {
        return NULL;
    }

    if (RET_SUCCEEDED(JsonTokenizer_Parse(&tokenizer, string)))
    {
        object = JsonTokenizer_ConvertToObject(&tokenizer);
    }

    JsonTokenizer_Dispose(&tokenizer);

    return object;
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

TINY_LOR
TinyRet JsonObject_Encode(JsonObject *thiz, bool pretty)
{
    int length = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    length = JsonObject_ToString(thiz, pretty, 0, NULL, 0, 0);
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

    JsonObject_ToString(thiz, pretty, 0, thiz->string, (uint32_t)length, 0);

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
JsonValue * JsonObject_Get(JsonObject *thiz, const char *key)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

	return (JsonValue *) TinyMap_GetValue(&thiz->data, key);
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

        ret = TinyMap_Insert(&thiz->data, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyMap_Insert FAILED!");
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

        ret = TinyMap_Insert(&thiz->data, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyMap_Insert FAILED!");
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

        ret = TinyMap_Insert(&thiz->data, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyMap_Insert FAILED!");
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

        ret = TinyMap_Insert(&thiz->data, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyMap_Insert FAILED!");
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

        ret = TinyMap_Insert(&thiz->data, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyMap_Insert FAILED!");
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

        ret = TinyMap_Insert(&thiz->data, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyMap_Insert FAILED!");
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

        ret = TinyMap_Insert(&thiz->data, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyMap_Insert FAILED!");
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

        ret = TinyMap_Insert(&thiz->data, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyMap_Insert FAILED!");
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

    return TinyMap_Insert(&thiz->data, key, value);
}