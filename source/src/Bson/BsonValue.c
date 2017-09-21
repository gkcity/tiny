/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonValue.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "BsonValue.h"
#include "BsonObject.h"
#include "BsonArray.h"

#define TAG     "BsonValue"


static
TinyRet BsonValue_Construct(BsonValue *thiz)
{
    memset(thiz, 0, sizeof(BsonValue));
    thiz->type = BSON_UNDEFINED;

    return TINY_RET_OK;
}

TINY_LOR
BsonValue * BsonValue_New(void)
{
    BsonValue * thiz = NULL;

    do
    {
        thiz = (BsonValue *) tiny_malloc(sizeof(BsonValue));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(BsonValue_Construct(thiz)))
        {
            LOG_E(TAG, "BsonValue_Construct FAILED");
            BsonValue_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

#if 0
TINY_LOR
BsonValue * BsonValue_NewString(const char *value)
{
    BsonValue * thiz = BsonValue_New();
    if (thiz != NULL)
    {
        thiz->type = BSON_STRING;
        thiz->data.string = BsonString_New(value);
        if (thiz->data.string == NULL)
        {
            BsonValue_Delete(thiz);
            thiz = NULL;
        }
    }

    return thiz;
}

TINY_LOR
BsonValue * BsonValue_NewInteger(int value)
{
    BsonValue * thiz = BsonValue_New();
    if (thiz != NULL)
    {
        thiz->type = BSON_NUMBER;
        thiz->data.number = BsonNumber_NewInteger(value);
        if (thiz->data.number == NULL)
        {
            BsonValue_Delete(thiz);
            thiz = NULL;
        }
    }

    return thiz;
}

TINY_LOR
BsonValue * BsonValue_NewFloat(float value)
{
    BsonValue * thiz = BsonValue_New();
    if (thiz != NULL)
    {
        thiz->type = BSON_NUMBER;
        thiz->data.number = BsonNumber_NewFloat(value);
        if (thiz->data.number == NULL)
        {
            BsonValue_Delete(thiz);
            thiz = NULL;
        }
    }

    return thiz;
}

TINY_LOR
BsonValue * BsonValue_NewBoolean(bool value)
{
    BsonValue * thiz = BsonValue_New();
    if (thiz != NULL)
    {
        thiz->type = value ? BSON_TRUE : BSON_FALSE;
    }

    return thiz;
}

TINY_LOR
BsonValue * BsonValue_NewNull()
{
    BsonValue * thiz = BsonValue_New();
    if (thiz != NULL)
    {
        thiz->type = BSON_NULL;
    }

    return thiz;
}
#endif

TINY_LOR
BsonValue * BsonValue_NewValue(BsonValueType type, void *value)
{
    BsonValue * thiz = NULL;

    if (value != NULL)
    {
        thiz = BsonValue_New();
        if (thiz != NULL)
        {
            thiz->type = type;

            switch (type)
            {
                case BSON_FLOAT:
                    thiz->data.floatingPoint = (BsonFloatingPoint *) value;
                    break;

                case BSON_STRING:
                    thiz->data.string = (BsonString *) value;
                    break;

                case BSON_OBJECT:
                    thiz->data.object = (BsonObject *) value;
                    break;

                case BSON_ARRAY:
                    thiz->data.array = (BsonArray *) value;
                    break;

                case BSON_BINARY:
                    thiz->data.binary = (BsonBinary *) value;
                    break;

                case BSON_BOOLEAN:
                    thiz->data.boolean = (BsonBoolean *) value;
                    break;

                case BSON_DATETIME:
                    thiz->data.datetime = (BsonDateTime *) value;
                    break;

                case BSON_INT32:
                    thiz->data.int32 = (BsonInt32 *) value;
                    break;

                case BSON_TIMESTAMP:
                    thiz->data.timestamp = (BsonTimestamp *) value;
                    break;

                case BSON_INT64:
                    thiz->data.int64 = (BsonInt64 *) value;
                    break;

                case BSON_UNDEFINED:
                case BSON_REVERSED:
                case BSON_OBJECT_ID:
                case BSON_NULL:
                case BSON_CSTRING:
                case BSON_DBPOINTER:
                case BSON_JAVASCRIPT_CODE:
                case BSON_SYMBOL:
                case BSON_JAVASCRIPT_CODE_SCOPE:
                case BSON_DECIMAL128:
                case BSON_MIN_KEY:
                case BSON_MAX_KEY:
                    BsonValue_Delete(thiz);
                    thiz = NULL;
                    break;
            }
        }
    }

    return thiz;
}

TINY_LOR
void BsonValue_Delete(BsonValue *thiz)
{
    switch (thiz->type)
    {
        case BSON_FLOAT:
            if (thiz->data.floatingPoint != NULL)
            {
                BsonFloatingPoint_Delete(thiz->data.floatingPoint);
            }
            break;

        case BSON_STRING:
            if (thiz->data.string != NULL)
            {
                BsonString_Delete(thiz->data.string);
            }
            break;

        case BSON_OBJECT:
            if (thiz->data.object != NULL)
            {
                BsonObject_Delete(thiz->data.object);
            }
            break;

        case BSON_ARRAY:
            if (thiz->data.array != NULL)
            {
                BsonArray_Delete(thiz->data.array);
            }
            break;

        case BSON_BINARY:
            if (thiz->data.binary != NULL)
            {
                BsonBinary_Delete(thiz->data.binary);
            }
            break;

        case BSON_BOOLEAN:
            if (thiz->data.boolean != NULL)
            {
                BsonBoolean_Delete(thiz->data.boolean);
            }
            break;

        case BSON_DATETIME:
            if (thiz->data.datetime != NULL)
            {
                BsonDateTime_Delete(thiz->data.datetime);
            }
            break;

        case BSON_INT32:
            if (thiz->data.int32 != NULL)
            {
                BsonInt32_Delete(thiz->data.int32);
            }
            break;

        case BSON_TIMESTAMP:
            if (thiz->data.timestamp != NULL)
            {
                BsonTimestamp_Delete(thiz->data.timestamp);
            }
            break;

        case BSON_INT64:
            if (thiz->data.int64 != NULL)
            {
                BsonInt64_Delete(thiz->data.int64);
            }
            break;

        case BSON_UNDEFINED:
        case BSON_REVERSED:
        case BSON_OBJECT_ID:
        case BSON_NULL:
        case BSON_CSTRING:
        case BSON_DBPOINTER:
        case BSON_JAVASCRIPT_CODE:
        case BSON_SYMBOL:
        case BSON_JAVASCRIPT_CODE_SCOPE:
        case BSON_DECIMAL128:
        case BSON_MIN_KEY:
        case BSON_MAX_KEY:
            break;
    }

    tiny_free(thiz);
}
