/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonDateTime.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "BsonDateTime.h"

#define TAG     "BsonDateTime"

TINY_LOR
static TinyRet BsonDateTime_Construct(BsonDateTime *thiz, const uint8_t *v)
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
BsonDateTime * BsonDateTime_New(const uint8_t *value)
{
    BsonDateTime *thiz = NULL;

    do
    {
        thiz = (BsonDateTime *) tiny_malloc(sizeof(BsonDateTime));
        if (thiz == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED!");
            break;
        }

        if (RET_FAILED(BsonDateTime_Construct(thiz, value)))
        {
            LOG_E(TAG, "BsonDateTime_Construct FAILED");
            BsonDateTime_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void BsonDateTime_Delete(BsonDateTime *thiz)
{
    tiny_free(thiz);
}