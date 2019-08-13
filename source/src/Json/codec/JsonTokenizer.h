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
TinyRet JsonTokenizer_Construct(_IN_ JsonTokenizer * thiz);

TINY_LOR
void JsonTokenizer_Dispose(_IN_ JsonTokenizer * thiz);

TINY_LOR
TinyRet JsonTokenizer_Parse(_IN_ JsonTokenizer * thiz, _IN_ const char *string, _IN_ bool parseOnce);

TINY_LOR
JsonToken * JsonTokenizer_Head(_IN_ JsonTokenizer * thiz);

TINY_LOR
void JsonTokenizer_Pop(_IN_ JsonTokenizer * thiz);


TINY_END_DECLS

#endif /* __JSON_TOKENIZER_H__ */