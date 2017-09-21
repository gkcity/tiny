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

#ifndef __JSON_DECODER_H__
#define __JSON_DECODER_H__

#include <tiny_base.h>
#include <TinyList.h>
#include "JsonObject.h"

TINY_BEGIN_DECLS


typedef struct _JsonDecoder
{
    TinyList        tokens;
    const char    * string;
    const char    * current;
    uint32_t        index;
} JsonDecoder;

TINY_LOR
TinyRet JsonDecoder_Construct(JsonDecoder *thiz);

TINY_LOR
void JsonDecoder_Dispose(JsonDecoder *thiz);

TINY_LOR
TinyRet JsonDecoder_Parse(JsonDecoder *thiz, const char *string);

TINY_LOR
JsonObject * JsonDecoder_ConvertToObject(JsonDecoder *thiz);

#ifdef TINY_DEBUG
void JsonDecoder_Print(JsonDecoder *thiz);
#else
#define JsonDecoder_Print(t)
#endif


TINY_END_DECLS

#endif /* __JSON_DECODER_H__ */