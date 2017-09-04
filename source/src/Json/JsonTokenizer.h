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
#include <TinyList.h>

TINY_BEGIN_DECLS

typedef struct _JsonTokenizer
{
    TinyList        tokens;
    const char    * string;
    const char    * current;
} JsonTokenizer;

TINY_LOR
TinyRet JsonTokenizer_Construct(JsonTokenizer *thiz);

TINY_LOR
void JsonTokenizer_Dispose(JsonTokenizer *thiz);

TINY_LOR
TinyRet JsonTokenizer_Parse(JsonTokenizer *thiz, const char *string);

#ifdef TINY_DEBUG
void JsonTokenizer_Print(JsonTokenizer *thiz);
#endif


TINY_END_DECLS

#endif /* __JSON_TOKENIZER_H__ */