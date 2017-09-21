/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   BsonJavascriptCodeScope.h
 *
 * @remark
 *
 */

#ifndef __BSON_JAVASCRIPT_CODE_SCOPE_H__
#define __BSON_JAVASCRIPT_CODE_SCOPE_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _BsonJavascriptCodeScope
{
    char        * value;
    uint32_t      length;
} BsonJavascriptCodeScope;

TINY_LOR
BsonJavascriptCodeScope * BsonJavascriptCodeScope_New(const uint8_t *value);

TINY_LOR
void BsonJavascriptCodeScope_Delete(BsonJavascriptCodeScope *thiz);


TINY_END_DECLS

#endif /* __BSON_JAVASCRIPT_CODE_SCOPE_H__ */