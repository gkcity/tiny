/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonInt32.h
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "BsonInt32.h"

#define TAG     "BsonInt32"

TINY_LOR
static TinyRet BsonInt32_Construct(BsonInt32 *thiz, const uint8_t * v)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->value =
            (uint32_t)(v[0]) +
            (((uint32_t)(v[1])) << 8) +
            (((uint32_t)(v[2])) << 16) +
            (((uint32_t)(v[3])) << 24);

    return TINY_RET_OK;
}

TINY_LOR
BsonInt32 * BsonInt32_New(const uint8_t *value)
{
    BsonInt32 *thiz = NULL;

    do
    {
        thiz = (BsonInt32 *) tiny_malloc(sizeof(BsonInt32));
        if (thiz == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED!");
            break;
        }

        if (RET_FAILED(BsonInt32_Construct(thiz, value)))
        {
            LOG_E(TAG, "BsonInt32_Construct FAILED");
            BsonInt32_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void BsonInt32_Delete(BsonInt32 *thiz)
{
    tiny_free(thiz);
}