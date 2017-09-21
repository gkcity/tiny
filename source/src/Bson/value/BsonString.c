/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonString.h
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "BsonString.h"

#define TAG     "BsonString"

TINY_LOR
static TinyRet BsonString_Construct(BsonString *thiz, const char *value)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(BsonString));

    if (value != NULL)
    {
        thiz->length = (uint32_t) strlen(value) + 1;
        thiz->value = (char *) tiny_malloc(thiz->length);
        if (thiz->value == NULL)
        {
            return TINY_RET_E_NEW;
        }

        memset(thiz->value, 0, thiz->length);
        strncpy(thiz->value, value, thiz->length);
    }

    return TINY_RET_OK;
}

TINY_LOR
static void BsonString_Dispose(BsonString *thiz)
{
    if (thiz->value != NULL)
    {
        tiny_free(thiz->value);
        thiz->value = NULL;
        thiz->length = 0;
    }
}

TINY_LOR
BsonString * BsonString_New(const char *value)
{
    BsonString *thiz = NULL;

    do
    {
        thiz = (BsonString *) tiny_malloc(sizeof(BsonString));
        if (thiz == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED!");
            break;
        }

        if (RET_FAILED(BsonString_Construct(thiz, value)))
        {
            LOG_E(TAG, "BsonString_Construct FAILED");
            BsonString_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void BsonString_Delete(BsonString *thiz)
{
    BsonString_Dispose(thiz);
    tiny_free(thiz);
}