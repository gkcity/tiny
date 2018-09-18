/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpRequestHandlerContext.c
 *
 * @remark
 *
 */

#include <tiny_malloc.h>
#include "HttpRequestHandlerContext.h"

TINY_LOR
HttpRequestHandlerContext * HttpRequestHandlerContext_New(const char *method, const char *uri, HttpRequestHandler handler, void *ctx)
{
    HttpRequestHandlerContext *thiz = NULL;

    RETURN_VAL_IF_FAIL(method, NULL);
    RETURN_VAL_IF_FAIL(uri, NULL);
    RETURN_VAL_IF_FAIL(handler, NULL);

    thiz = (HttpRequestHandlerContext *)tiny_malloc(sizeof(HttpRequestHandlerContext));
    if (thiz != NULL)
    {
        memset(thiz, 0, sizeof(HttpRequestHandlerContext));
        strncpy(thiz->method, method, HTTP_METHOD_LEN);
        strncpy(thiz->uri, uri, TINY_URI_LEN);
        thiz->handler = handler;
        thiz->ctx = ctx;
    }

    return thiz;
}

TINY_LOR
void HttpRequestHandlerContext_Delete(HttpRequestHandlerContext *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(HttpRequestHandlerContext));
    tiny_free(thiz);
}