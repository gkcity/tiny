/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonBoolean.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "JsonBoolean.h"

#define TAG "JsonBoolean"

TINY_LOR
JsonBoolean * JsonBoolean_New(bool value)
{
    JsonBoolean *thiz = NULL;

    do
    {
        thiz = (JsonBoolean *) tiny_malloc(sizeof(JsonBoolean));
        if (thiz == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED!");
            break;
        }

        thiz->value = value;
    } while (0);

    return thiz;
}

TINY_LOR
void JsonBoolean_Delete(JsonBoolean *thiz)
{
    tiny_free(thiz);
}