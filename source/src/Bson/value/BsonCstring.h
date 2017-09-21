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

#ifndef __BSON_CSTRING_H__
#define __BSON_CSTRING_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonCstring
{
    char        * value;
    uint32_t      length;
} BsonCstring;

TINY_LOR
BsonCstring * BsonCstring_New(const uint8_t *value, uint32_t length);

TINY_LOR
void BsonCstring_Delete(BsonCstring *thiz);


TINY_END_DECLS

#endif /* __BSON_CSTRING_H__ */