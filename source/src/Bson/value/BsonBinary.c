/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonBinary.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "BsonBinary.h"

#define TAG     "BsonBinary"

TINY_LOR
static TinyRet BsonBinary_Construct(BsonBinary *thiz, BsonBinaryType type, const uint8_t *value, uint32_t length)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(BsonBinary));

    if (value != NULL)
    {
        thiz->type = type;
        thiz->length = length;
        thiz->value = (uint8_t *) tiny_malloc(thiz->length);
        if (thiz->value == NULL)
        {
            return TINY_RET_E_NEW;
        }

        memcpy(thiz->value, value, thiz->length);
    }

    return TINY_RET_OK;
}

TINY_LOR
static void BsonBinary_Dispose(BsonBinary *thiz)
{
    if (thiz->value != NULL)
    {
        tiny_free(thiz->value);
        thiz->value = NULL;
        thiz->length = 0;
    }
}

TINY_LOR
BsonBinary * BsonBinary_New(BsonBinaryType type, const uint8_t *value, uint32_t length)
{
    BsonBinary *thiz = NULL;

    do
    {
        thiz = (BsonBinary *) tiny_malloc(sizeof(BsonBinary));
        if (thiz == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED!");
            break;
        }

        if (RET_FAILED(BsonBinary_Construct(thiz, type, value, length)))
        {
            LOG_E(TAG, "BsonBinary_Construct FAILED");
            BsonBinary_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void BsonBinary_Delete(BsonBinary *thiz)
{
    BsonBinary_Dispose(thiz);
    tiny_free(thiz);
}