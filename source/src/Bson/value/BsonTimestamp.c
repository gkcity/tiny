/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonTimestamp.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "BsonTimestamp.h"

#define TAG     "BsonTimestamp"

TINY_LOR
static TinyRet BsonTimestamp_Construct(BsonTimestamp *thiz, const uint8_t *v)
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
BsonTimestamp * BsonTimestamp_New(const uint8_t *value)
{
    BsonTimestamp *thiz = NULL;

    do
    {
        thiz = (BsonTimestamp *) tiny_malloc(sizeof(BsonTimestamp));
        if (thiz == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED!");
            break;
        }

        if (RET_FAILED(BsonTimestamp_Construct(thiz, value)))
        {
            LOG_E(TAG, "BsonTimestamp_Construct FAILED");
            BsonTimestamp_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void BsonTimestamp_Delete(BsonTimestamp *thiz)
{
    tiny_free(thiz);
}