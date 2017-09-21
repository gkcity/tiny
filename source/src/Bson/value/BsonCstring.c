/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonCstring.h
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include "BsonCstring.h"

TINY_LOR
BsonCstring * BsonCstring_New(const uint8_t *value, uint32_t length)
{
    BsonCstring *thiz = NULL;

    return thiz;
}

TINY_LOR
void BsonCstring_Delete(BsonCstring *thiz)
{
    tiny_free(thiz);
}