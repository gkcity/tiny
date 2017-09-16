/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpHeader.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#ifndef __HTTP_HEADER_H__
#define __HTTP_HEADER_H__

#include <tiny_base.h>
#include <tiny_api.h>
#include <TinyMap.h>
#include "Bytes.h"

TINY_BEGIN_DECLS


#define CONTENT_LENGTH      "Content-Length"

typedef struct _HttpHeader
{
    TinyMap         values;
} HttpHeader;

TINY_API
TINY_LOR
TinyRet HttpHeader_Construct(HttpHeader *thiz);

TINY_API
TINY_LOR
TinyRet HttpHeader_Dispose(HttpHeader *thiz);

//TINY_LOR
//HttpHeader * HttpHeader_New(void);
//
//TINY_LOR
//void HttpHeader_Delete(HttpHeader *thiz);

TINY_API
TINY_LOR
TinyRet HttpHeader_Set(HttpHeader * thiz, const char *name, const char *value);

TINY_API
TINY_LOR
TinyRet HttpHeader_SetInteger(HttpHeader * thiz, const char *name, uint32_t value);

TINY_API
TINY_LOR
TinyRet HttpHeader_SetHost(HttpHeader * thiz, const char *name, uint16_t port);

TINY_API
TINY_LOR
const char * HttpHeader_GetValue(HttpHeader * thiz, const char *name);

TINY_API
TINY_LOR
uint32_t HttpHeader_GetSize(HttpHeader * thiz);

TINY_API
TINY_LOR
TinyRet HttpHeader_GetContentLength(HttpHeader * thiz, uint32_t *length);

TINY_API
TINY_LOR
TinyRet HttpHeader_Parse(HttpHeader *thiz, Bytes *bytes);


TINY_END_DECLS

#endif /* __HTTP_HEADER_H__ */
