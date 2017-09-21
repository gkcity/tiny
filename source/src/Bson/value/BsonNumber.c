/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonNumber.h
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "BsonNumber.h"

#define TAG     "BsonNumber"

TINY_LOR
BsonNumber * BsonNumber_NewInteger(int value)
{
    BsonNumber *thiz = NULL;

    do
    {
        thiz = (BsonNumber *) tiny_malloc(sizeof(BsonNumber));
        if (thiz == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED!");
            break;
        }

        thiz->type = JSON_NUMBER_INTEGER;
        thiz->value.intValue = value;
    } while (0);

    return thiz;
}

TINY_LOR
BsonNumber * BsonNumber_NewFloat(float value)
{
    BsonNumber *thiz = NULL;

    do
    {
        thiz = (BsonNumber *) tiny_malloc(sizeof(BsonNumber));
        if (thiz == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED!");
            break;
        }

        thiz->type = JSON_NUMBER_FLOAT;
        thiz->value.floatValue = value;
    } while (0);

    return thiz;
}

TINY_LOR
void BsonNumber_Delete(BsonNumber *thiz)
{
    tiny_free(thiz);
}