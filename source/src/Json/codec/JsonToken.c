/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonToken.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "JsonToken.h"

#define TAG     "JsonToken"

TINY_LOR
JsonToken * JsonToken_New(JsonTokenType type, uint32_t offset, uint32_t length)
{
    JsonToken *thiz = tiny_malloc(sizeof(JsonToken));
    if (thiz == NULL)
    {
        LOG_E(TAG, "tiny_malloc failed!");
        return NULL;
    }

    thiz->type = type;
    thiz->offset = offset;
    thiz->length = length;

    return thiz;
}

TINY_LOR
void JsonToken_Delete(JsonToken *thiz)
{
    tiny_free(thiz);
}