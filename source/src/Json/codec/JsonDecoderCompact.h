/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonDecoderCompact.h
 *
 * @remark
 *
 */

#ifndef __JSON_DECODER_COMPACT_H__
#define __JSON_DECODER_COMPACT_H__

#include <tiny_base.h>
#include "JsonTokenizer.h"
#include "JsonObject.h"

TINY_BEGIN_DECLS


typedef struct _JsonDecoderCompact
{
    JsonTokenizer   tokenizer;

//    const char    * string;
//    const char    * current;
//    uint32_t        index;
} JsonDecoderCompact;

TINY_LOR
TinyRet JsonDecoderCompact_Construct(JsonDecoderCompact *thiz);

TINY_LOR
void JsonDecoderCompact_Dispose(JsonDecoderCompact *thiz);

TINY_LOR
JsonObject * JsonDecoderCompact_Parse(JsonDecoderCompact *thiz, const char *string);


TINY_END_DECLS

#endif /* __JSON_DECODER_COMPACT_H__ */