/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonNumber.h
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "JsonNumber.h"

#define TAG     "JsonNumber"

TINY_LOR
JsonNumber * JsonNumber_NewInteger(int value)
{
    JsonNumber *thiz = NULL;

    do
    {
        thiz = (JsonNumber *) tiny_malloc(sizeof(JsonNumber));
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
JsonNumber * JsonNumber_NewFloat(float value)
{
    JsonNumber *thiz = NULL;

    do
    {
        thiz = (JsonNumber *) tiny_malloc(sizeof(JsonNumber));
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
void JsonNumber_Delete(JsonNumber *thiz)
{
    tiny_free(thiz);
}