/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonTokenizer.h
 *
 * @remark
 *
 */

#ifndef __JSON_TOKENIZER_H__
#define __JSON_TOKENIZER_H__

#include <tiny_base.h>
#include <TinyQueue.h>
#include "JsonToken.h"

TINY_BEGIN_DECLS


typedef struct _JsonTokenizer
{
    TinyQueue       queue;
    const char    * string;
    const char    * current;
    bool            parseOnce;
} JsonTokenizer;

TINY_LOR
TinyRet JsonTokenizer_Construct(JsonTokenizer * thiz);

TINY_LOR
void JsonTokenizer_Dispose(JsonTokenizer * thiz);

TINY_LOR
TinyRet JsonTokenizer_Parse(JsonTokenizer * thiz, const char *string, bool parseOnce);

TINY_LOR
JsonToken * JsonTokenizer_Head(JsonTokenizer * thiz);

TINY_LOR
void JsonTokenizer_Pop(JsonTokenizer * thiz);


TINY_END_DECLS

#endif /* __JSON_TOKENIZER_H__ */