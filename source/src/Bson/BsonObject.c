/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonObject.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <TinyMapItem.h>
#include <codec/StringEncoder.h>
#include "BsonObject.h"
#include "codec/BsonEncoder.h"
#include "codec/BsonDecoder.h"

#define TAG     "BsonObject"

TINY_LOR
static void _OnBsonValueDelete (void * data, void *ctx)
{
    BsonValue_Delete((BsonValue *)data);
}

TINY_LOR
BsonObject * BsonObject_New(void)
{
	BsonObject * thiz = NULL;

	do
	{
		thiz = (BsonObject *) tiny_malloc(sizeof(BsonObject));
		if (thiz == NULL)
		{
			break;
		}

        if (RET_FAILED(BsonObject_Construct(thiz)))
        {
            LOG_E(TAG, "BsonObject_Construct FAILED");
            BsonObject_Delete(thiz);
            thiz = NULL;
            break;
        }
	} while (false);

	return thiz;
}

TINY_LOR
void BsonObject_Delete(BsonObject *thiz)
{
    BsonObject_Dispose(thiz);
	tiny_free(thiz);
}

TINY_LOR
TinyRet BsonObject_Construct(BsonObject *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do 
    {
        memset(thiz, 0, sizeof(BsonObject));
        thiz->binary = NULL;
        thiz->size = 0;

        ret = TinyMap_Construct(&thiz->data);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyMap_SetDeleteListener(&thiz->data, _OnBsonValueDelete, thiz);
    } while (0);

    return ret;
}

TINY_LOR
void BsonObject_Dispose(BsonObject *thiz)
{
    RETURN_IF_FAIL(thiz);

    if (thiz->binary != NULL)
    {
        tiny_free(thiz->binary);
        thiz->binary = NULL;
    }

    TinyMap_Dispose(&thiz->data);
}

TINY_API
BsonObject * BsonObject_NewBinary(const uint8_t *binary, uint32_t length)
{
    BsonObject *object = NULL;

    RETURN_VAL_IF_FAIL(binary, NULL);

    do
    {
        BsonDecoder decoder;

        if (RET_FAILED(BsonDecoder_Construct(&decoder, binary, 0, length)))
        {
            LOG_D(TAG, "BsonDecoder_Construct failed");
            break;
        }

        if (RET_FAILED(BsonDecoder_Parse(&decoder)))
        {
            LOG_D(TAG, "BsonDecoder_Parse failed");
            BsonDecoder_Dispose(&decoder);
            break;
        }

        object = BsonDecoder_ConvertToObject(&decoder);
        BsonDecoder_Dispose(&decoder);
    } while (false);

    return object;
}

TINY_LOR
TinyRet BsonObject_Encode(BsonObject *thiz)
{
    int length = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    length = BsonEncoder_EncodeObject(thiz, NULL, 0, 0);
    if (length < 0)
    {
        return TINY_RET_E_INTERNAL;
    }

    if (thiz->binary != NULL)
    {
        tiny_free(thiz->binary);
    }

    thiz->binary = tiny_malloc((size_t)length);
    if (thiz->binary == NULL)
    {
        return TINY_RET_E_NEW;
    }

    memset(thiz->binary, 0, (size_t)length);
    thiz->size = (uint32_t)length;

    BsonEncoder_EncodeObject(thiz, thiz->binary, (uint32_t)length, 0);

    return TINY_RET_OK;
}

#ifdef TINY_DEBUG
TINY_LOR
TINY_API
TinyRet BsonObject_ToString(BsonObject *thiz)
{
    int length = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    length = StringEncoder_EncodeObject(thiz, true, 0, NULL, 0, 0);
    if (length < 0)
    {
        return TINY_RET_E_INTERNAL;
    }

    if (thiz->binary != NULL)
    {
        tiny_free(thiz->binary);
    }

    thiz->binary = tiny_malloc((size_t)length + 1);
    if (thiz->binary == NULL)
    {
        return TINY_RET_E_NEW;
    }

    memset(thiz->binary, 0, (size_t)length + 1);
    thiz->size = (uint32_t)length;

    StringEncoder_EncodeObject(thiz, true, 0, (char *)thiz->binary, (uint32_t)length, 0);

    return TINY_RET_OK;
}
#endif /* TINY_DEBUG */

TINY_LOR
bool BsonObject_ContainsKey(BsonObject *thiz, const char *key)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);

    return (TinyMap_GetValue(&thiz->data, key) != NULL);
}

TINY_LOR
BsonValue * BsonObject_GetValue(BsonObject *thiz, const char *key)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

	return (BsonValue *) TinyMap_GetValue(&thiz->data, key);
}

#if 0
TINY_LOR
BsonObject * BsonObject_GetObject(BsonObject *thiz, const char *key)
{
    BsonObject *object = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

    do
    {
        BsonValue *value = BsonObject_GetValue(thiz, key);
        if (value == NULL)
        {
            break;
        }

        if (value->type != BSON_OBJECT)
        {
            break;
        }

        object = value->data.object;
    } while (false);

    return object;
}

TINY_LOR
BsonNumber *BsonObject_GetNumber(BsonObject *thiz, const char *key)
{
    BsonNumber *number = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

    do
    {
        BsonValue *value = BsonObject_GetValue(thiz, key);
        if (value == NULL)
        {
            break;
        }

        if (value->type != BSON_NUMBER)
        {
            break;
        }

        number = value->data.number;
    } while (false);

    return number;
}

TINY_LOR
BsonArray *BsonObject_GetArray(BsonObject *thiz, const char *key)
{
    BsonArray *array = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

    do
    {
        BsonValue *value = BsonObject_GetValue(thiz, key);
        if (value == NULL)
        {
            break;
        }

        if (value->type != BSON_ARRAY)
        {
            break;
        }

        array = value->data.array;
    } while (false);

    return array;
}

TINY_LOR
BsonString *BsonObject_GetString(BsonObject *thiz, const char *key)
{
    BsonString *string = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(key, NULL);

    do
    {
        BsonValue *value = BsonObject_GetValue(thiz, key);
        if (value == NULL)
        {
            break;
        }

        if (value->type != BSON_STRING)
        {
            break;
        }

        string = value->data.string;
    } while (false);

    return string;
}
#endif

TINY_LOR
TinyRet BsonObject_PutString(BsonObject *thiz, const char *key, const char *value)
{
	TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        BsonValue * v = BsonValue_NewString(value);
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewString FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = BsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "BsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet BsonObject_PutInt32(BsonObject *thiz, const char *key, int32_t value)
{
	TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);

    do
    {
        BsonValue * v = BsonValue_NewInt32(value);
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewInt32 FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = BsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "BsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet BsonObject_PutFloat(BsonObject *thiz, const char *key, float value)
{
	TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);

    do
    {
        BsonValue * v = BsonValue_NewFloat(value);
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewFloat FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = BsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "BsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet BsonObject_PutBoolean(BsonObject *thiz, const char *key, bool value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);

    do
    {
        BsonValue * v = BsonValue_NewBoolean(value);
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewBoolean FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = BsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "BsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

#if 0
TINY_LOR
TinyRet BsonObject_PutNull(BsonObject *thiz, const char *key)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);

    do
    {
        BsonValue * v = BsonValue_NewNull();
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewNull FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = BsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "BsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet BsonObject_PutObject(BsonObject *thiz, const char *key, BsonObject *value)
{
	TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        BsonValue * v = BsonValue_NewValue(BSON_OBJECT, value);
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewObject FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = BsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "BsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet BsonObject_PutArray(BsonObject *thiz, const char *key, BsonArray *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        BsonValue * v = BsonValue_NewValue(BSON_ARRAY, value);
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewArray FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = BsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "BsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet BsonObject_PutNumber(BsonObject *thiz, const char *key, BsonNumber *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        BsonValue * v = BsonValue_NewValue(BSON_NUMBER, value);
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewArray FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = BsonObject_PutValue(thiz, key, v);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "BsonObject_PutValue FAILED: %d", TINY_RET_CODE(ret));
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}
#endif

TINY_LOR
TinyRet BsonObject_PutValue(BsonObject *thiz, const char *key, BsonValue *value)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(key, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    TinyMap_Erase(&thiz->data, key);

    return TinyMap_Insert(&thiz->data, key, value);
}