/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonInt64.h
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "BsonInt64.h"

#define TAG     "BsonInt64"

TINY_LOR
static TinyRet BsonInt64_Construct(BsonInt64 *thiz, const uint8_t * v)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->value =
            (uint64_t)(v[0]) +
            (((uint64_t)(v[1])) << 8) +
            (((uint64_t)(v[2])) << 16) +
            (((uint64_t)(v[3])) << 24) +
            (((uint64_t)(v[3])) << 32) +
            (((uint64_t)(v[3])) << 40) +
            (((uint64_t)(v[3])) << 48) +
            (((uint64_t)(v[3])) << 56);

    return TINY_RET_OK;
}

TINY_LOR
BsonInt64 * BsonInt64_New(const uint8_t *value)
{
    BsonInt64 *thiz = NULL;

    do
    {
        thiz = (BsonInt64 *) tiny_malloc(sizeof(BsonInt64));
        if (thiz == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED!");
            break;
        }

        if (RET_FAILED(BsonInt64_Construct(thiz, value)))
        {
            LOG_E(TAG, "BsonInt64_Construct FAILED");
            BsonInt64_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void BsonInt64_Delete(BsonInt64 *thiz)
{
    tiny_free(thiz);
}