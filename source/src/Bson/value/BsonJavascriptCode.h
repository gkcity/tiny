/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonJavascriptCode.h
 *
 * @remark
 *
 */

#ifndef __BSON_JAVASCRIPT_CODE_H__
#define __BSON_JAVASCRIPT_CODE_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonJavascriptCode
{
    char        * value;
    uint32_t      length;
} BsonJavascriptCode;

TINY_LOR
BsonJavascriptCode * BsonJavascriptCode_New(const uint8_t *value);

TINY_LOR
void BsonJavascriptCode_Delete(BsonJavascriptCode *thiz);


TINY_END_DECLS

#endif /* __BSON_JAVASCRIPT_CODE_H__ */