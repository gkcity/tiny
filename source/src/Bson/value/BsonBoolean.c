/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonBoolean.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "BsonBoolean.h"

#define TAG     "BsonBoolean"

TINY_LOR
static TinyRet BsonBoolean_Construct(BsonBoolean *thiz, uint8_t value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    switch (value)
    {
        case 0x00:
            thiz->value = false;
            break;

        case 0x01:
            thiz->value = true;
            break;

        default:
            ret = TINY_RET_E_ARG_INVALID;
            break;
    }

    return ret;
}

TINY_LOR
BsonBoolean * BsonBoolean_New(uint8_t value)
{
    BsonBoolean *thiz = NULL;

    do
    {
        thiz = (BsonBoolean *) tiny_malloc(sizeof(BsonBoolean));
        if (thiz == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED!");
            break;
        }

        if (RET_FAILED(BsonBoolean_Construct(thiz, value)))
        {
            LOG_E(TAG, "BsonBoolean_Construct FAILED");
            BsonBoolean_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void BsonBoolean_Delete(BsonBoolean *thiz)
{
    tiny_free(thiz);
}