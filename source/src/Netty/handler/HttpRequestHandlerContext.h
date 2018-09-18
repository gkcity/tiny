/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpRequestHandlerContext.h
 *
 * @remark
 *
 */

#ifndef __HTTP_REQUEST_HANDLER_CONTEXT_H__
#define __HTTP_REQUEST_HANDLER_CONTEXT_H__

#include <tiny_base.h>
#include <codec-http/HttpMessage.h>

TINY_BEGIN_DECLS


typedef HttpMessage* (*HttpRequestHandler) (HttpMessage *request, void *ctx);

typedef struct _HttpRequestHandlerContext
{
    char                    method[HTTP_METHOD_LEN];
    char                    uri[TINY_URI_LEN];
    HttpRequestHandler      handler;
    void                  * ctx;
} HttpRequestHandlerContext;

TINY_API
TINY_LOR
HttpRequestHandlerContext * HttpRequestHandlerContext_New(const char *method, const char *uri, HttpRequestHandler handler, void *ctx);

TINY_API
TINY_LOR
void HttpRequestHandlerContext_Delete(HttpRequestHandlerContext *thiz);


TINY_END_DECLS

#endif /* __HTTP_REQUEST_HANDLER_CONTEXT_H__ */