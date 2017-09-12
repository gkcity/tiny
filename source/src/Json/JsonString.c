/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonString.h
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "JsonString.h"

#define TAG     "JsonString"

TINY_LOR
static TinyRet JsonString_Construct(JsonString *thiz, const char *value)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(JsonString));

    if (value != NULL)
    {
        thiz->length = strlen(value) + 1;
        thiz->value = (char *) tiny_malloc(thiz->length);
        if (thiz->value == NULL)
        {
            return TINY_RET_E_NEW;
        }

        memset(thiz->value, 0, thiz->length);
        strncpy(thiz->value, value, thiz->length);
    }

    return TINY_RET_OK;
}

TINY_LOR
static void JsonString_Dispose(JsonString *thiz)
{
    if (thiz->value != NULL)
    {
        tiny_free(thiz->value);
        thiz->value = NULL;
        thiz->length = 0;
    }
}

TINY_LOR
JsonString * JsonString_New(const char *value)
{
    JsonString *thiz = NULL;

    do
    {
        thiz = (JsonString *) tiny_malloc(sizeof(JsonString));
        if (thiz == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED!");
            break;
        }

        if (RET_FAILED(JsonString_Construct(thiz, value)))
        {
            LOG_E(TAG, "JsonString_Construct FAILED");
            JsonString_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void JsonString_Delete(JsonString *thiz)
{
    JsonString_Dispose(thiz);
    tiny_free(thiz);
}