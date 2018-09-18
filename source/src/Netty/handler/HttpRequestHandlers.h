/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpRequestHandlers.h
 *
 * @remark
 *
 */

#ifndef __HTTP_REQUEST_HANDLERS_H__
#define __HTTP_REQUEST_HANDLERS_H__

#include <tiny_base.h>
#include <TinyList.h>
#include "HttpRequestHandlerContext.h"

TINY_BEGIN_DECLS


typedef TinyList HttpRequestHandlers;

TINY_API
TINY_LOR
HttpRequestHandlers * HttpRequestHandlers_New(void);

TINY_API
TINY_LOR
void HttpRequestHandlers_Delete(HttpRequestHandlers *thiz);

TINY_API
TINY_LOR
TinyRet HttpRequestHandlers_Put(HttpRequestHandlers *thiz, const char *method, const char *uri, HttpRequestHandler handler, void *ctx);

TINY_API
TINY_LOR
HttpRequestHandlerContext * HttpRequestHandlers_Get(HttpRequestHandlers *thiz, const char *method, const char *uri);


TINY_END_DECLS

#endif /* __HTTP_REQUEST_HANDLERS_H__ */