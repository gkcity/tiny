/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonBinary.h
 *
 * @remark
 *
 */

#ifndef __BSON_BINARY_H__
#define __BSON_BINARY_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef enum _BsonBinaryType
{
    BINARY_GENERIC          = 0x00,
    BINARY_FUNCION          = 0x01,
    BINARY_BINARY_OLD       = 0x02,
    BINARY_UUID_OLD         = 0x03,
    BINARY_UUID             = 0x04,
    BINARY_MD5              = 0x05,
    BINARY_USER_DEFINED     = 0x80,
} BsonBinaryType;

typedef struct _BsonBinary
{
    BsonBinaryType      type;
    uint8_t           * value;
    uint32_t            length;
} BsonBinary;

TINY_LOR
BsonBinary * BsonBinary_New(BsonBinaryType type, const uint8_t *value, uint32_t length);

TINY_LOR
void BsonBinary_Delete(BsonBinary *thiz);


TINY_END_DECLS

#endif /* __BSON_BINARY_H__ */