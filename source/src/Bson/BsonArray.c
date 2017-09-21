/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonArray.h
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "BsonArray.h"
#include "BsonObject.h"

#define TAG     "BsonArray"

TINY_LOR
static void _OnBsonValueDelete (void * data, void *ctx)
{
    BsonValue_Delete((BsonValue *)data);
}

TINY_LOR
static TinyRet BsonArray_Construct(BsonArray *thiz, BsonValueType type)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do 
    {
        memset(thiz, 0, sizeof(BsonArray));
        thiz->type = type;
        thiz->string = NULL;

        ret = TinyList_Construct(&thiz->values);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyList_SetDeleteListener(&thiz->values, _OnBsonValueDelete, thiz);
    } while (false);

    return ret;
}

TINY_LOR
static void BsonArray_Dispose(BsonArray *thiz)
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
BsonArray * BsonArray_New(BsonValueType type)
{
    BsonArray * thiz = NULL;

    do
    {
        thiz = (BsonArray *) tiny_malloc(sizeof(BsonArray));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(BsonArray_Construct(thiz, type)))
        {
            LOG_E(TAG, "BsonArray_Construct FAILED");
            BsonArray_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TINY_LOR
void BsonArray_Delete(BsonArray *thiz)
{
    BsonArray_Dispose(thiz);
    tiny_free(thiz);
}

#if 0
TINY_LOR
TinyRet BsonArray_Encode(BsonArray *thiz, bool pretty)
{
//    int length = 0;
//
//    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
//
//    length = BsonArray_ToString(thiz, pretty, 0, NULL, 0, 0);
//    if (length < 0)
//    {
//        return TINY_RET_E_INTERNAL;
//    }
//
//    if (thiz->string != NULL)
//    {
//        tiny_free(thiz->string);
//    }
//
//    thiz->string = tiny_malloc((size_t)length + 1);
//    if (thiz->string == NULL)
//    {
//        return TINY_RET_E_NEW;
//    }
//
//    memset(thiz->string, 0, (size_t)length + 1);
//
//    BsonArray_ToString(thiz, pretty, 0, thiz->string, (uint32_t)length, 0);
//
//    return TINY_RET_OK;

    return TINY_RET_E_NOT_IMPLEMENTED;
}

TINY_LOR
TinyRet BsonArray_AddString(BsonArray *thiz, const char *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->type != BSON_STRING)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        BsonValue * v = BsonValue_NewString(value);
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewString FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, v);
        if (RET_FAILED(ret))
        {
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet BsonArray_AddInteger(BsonArray *thiz, int value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->type != BSON_NUMBER)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        BsonValue * v = BsonValue_NewInteger(value);
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewInteger FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, v);
        if (RET_FAILED(ret))
        {
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet BsonArray_AddFloat(BsonArray *thiz, float value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->type != BSON_NUMBER)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        BsonValue * v = BsonValue_NewFloat(value);
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewInteger FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, v);
        if (RET_FAILED(ret))
        {
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet BsonArray_AddObject(BsonArray *thiz, BsonObject *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->type != BSON_OBJECT)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        BsonValue * v = BsonValue_NewValue(BSON_OBJECT, value);
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewInteger FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, v);
        if (RET_FAILED(ret))
        {
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet BsonArray_AddArray(BsonArray *thiz, BsonArray *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->type != BSON_ARRAY)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        BsonValue * v = BsonValue_NewValue(BSON_ARRAY, value);
        if (v == NULL)
        {
            LOG_E(TAG, "BsonValue_NewValue FAILED!");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, v);
        if (RET_FAILED(ret))
        {
            BsonValue_Delete(v);
        }
    } while (false);

    return ret;
}

TINY_LOR
TinyRet BsonArray_AddValue(BsonArray *thiz, BsonValue *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->type != value->type)
        {
            LOG_E(TAG, "BsonArray_AddValue FAILED, type invalid: %d", value->type);
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        ret = TinyList_AddTail(&thiz->values, value);
    } while (false);

    return ret;
}
#endif