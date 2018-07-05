/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   JsonDecoder.h
 *
 * @remark
 *
 */

#ifndef __JSON_DECODER_COMPACT_H__
#define __JSON_DECODER_COMPACT_H__

#include <tiny_base.h>
#include <tiny_api.h>
#include <tiny_lor.h>
#include "JsonTokenizer.h"
#include "JsonObject.h"

TINY_BEGIN_DECLS


typedef struct _JsonDecoder
{
    JsonTokenizer   tokenizer;
} JsonDecoder;

typedef enum _JsonDecoderMode
{
    JSON_DECODE_NORMAL      = 0,
    JSON_DECODE_LOW_MEMORY  = 1,
} JsonDecoderMode;

TINY_LOR
TINY_API
TinyRet JsonDecoder_Construct(JsonDecoder *thiz);

TINY_LOR
TINY_API
void JsonDecoder_Dispose(JsonDecoder *thiz);

TINY_LOR
TINY_API
JsonObject * JsonDecoder_Parse(JsonDecoder *thiz, const char *string, JsonDecoderMode mode);


TINY_END_DECLS

#endif /* __JSON_DECODER_COMPACT_H__ */