/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonFloatingPoint.h
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "BsonFloatingPoint.h"

#define TAG     "BsonFloatingPoint"

TINY_LOR
static TinyRet BsonFloatingPoint_Construct(BsonFloatingPoint *thiz, const uint8_t * value)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    if (value != NULL)
    {
        thiz->value = *((double *) value);
    }

    return TINY_RET_OK;
}

TINY_LOR
BsonFloatingPoint * BsonFloatingPoint_New(const uint8_t *value)
{
    BsonFloatingPoint *thiz = NULL;

    do
    {
        thiz = (BsonFloatingPoint *) tiny_malloc(sizeof(BsonFloatingPoint));
        if (thiz == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED!");
            break;
        }

        if (RET_FAILED(BsonFloatingPoint_Construct(thiz, value)))
        {
            LOG_E(TAG, "BsonFloatingPoint_Construct FAILED");
            BsonFloatingPoint_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void BsonFloatingPoint_Delete(BsonFloatingPoint *thiz)
{
    tiny_free(thiz);
}