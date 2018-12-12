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

#include <tiny_str_equal.h>
#include <tiny_str_split.h>
#include <StringArray.h>
#include <tiny_log.h>
#include "HttpRequestHandlers.h"

#define TAG "HttpRequestHandlers"

static void _OnItemRemoved (void * data, void *ctx)
{
    HttpRequestHandlerContext *item = (HttpRequestHandlerContext *)data;
    HttpRequestHandlerContext_Delete(item);
}

TINY_LOR
HttpRequestHandlers * HttpRequestHandlers_New(void)
{
    return TinyList_New(_OnItemRemoved, NULL);
}

TINY_LOR
void HttpRequestHandlers_Delete(HttpRequestHandlers *thiz)
{
    TinyList_Delete(thiz);
}

TINY_LOR
TinyRet HttpRequestHandlers_Put(HttpRequestHandlers *thiz, const char *method, const char *uri, HttpRequestHandler handler, void *ctx)
{
    TinyRet ret = TINY_RET_OK;
    HttpRequestHandlerContext *context = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(method, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(uri, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    for (uint32_t i = 0; i < thiz->size; ++i)
    {
        HttpRequestHandlerContext *item = (HttpRequestHandlerContext *) TinyList_GetAt(thiz, i);
        if (str_equal(item->method, method, true) && str_equal(item->uri, uri, true))
        {
            return TINY_RET_E_ITEM_EXIST;
        }
    }

    context = HttpRequestHandlerContext_New(method, uri, handler, ctx);
    if (context == NULL)
    {
        return TINY_RET_E_NEW;
    }

    ret = TinyList_AddTail(thiz, context);
    if (RET_FAILED(ret))
    {
        HttpRequestHandlerContext_Delete(context);
    }

    return ret;
}

TINY_LOR
TinyRet HttpRequestHandlers_PutDefaultHandler(HttpRequestHandlers *thiz, HttpRequestHandler handler, void *ctx)
{
    return HttpRequestHandlers_Put(thiz, "*", "*", handler, ctx);
}

TINY_LOR
static bool uriMatched(const char *pattern, const char *uri)
{
    bool ret = true;

    do
    {
        if (STR_EQUAL(pattern, uri) == 0)
        {
            break;
        }

        StringArray *p = StringArray_NewString(pattern, "/");
        StringArray *u = StringArray_NewString(uri, "/");

        if (p->values.size != u->values.size)
        {
            StringArray_Delete(p);
            StringArray_Delete(u);
            ret = false;
            break;
        }

        for (uint32_t i = 0; i < p->values.size; ++i)
        {
            const char *pp = TinyList_GetAt(&p->values, i);
            const char *uu = TinyList_GetAt(&u->values, i);

            LOG_D(TAG, "pp: %s == %s", pp, uu);

            if (pp[0] == ':')
            {
                LOG_D(TAG, "pp[0] = :, skip it");
                continue;
            }

            if (! str_equal(pp, uu, true))
            {
                ret = false;
                break;
            }
        }

    } while (false);

    return ret;
}

TINY_LOR
static HttpMessage * handleRequest(HttpRequestHandlers * thiz, HttpMessage *request)
{
    HttpRequestHandlerContext *c = HttpRequestHandlers_Get(thiz, request->request_line.method, request->request_line.uri);
    if (c == NULL)
    {
        return NULL;
    }

    return c->handler(request, c->ctx);
}

TINY_LOR
static HttpMessage * handleNotFound(HttpRequestHandlers * thiz, HttpMessage *request)
{
    HttpRequestHandlerContext *c = HttpRequestHandlers_Get(thiz, "*", "*");
    if (c == NULL)
    {
        return NULL;
    }

    return c->handler(request, c->ctx);
}

TINY_LOR
HttpMessage *HttpRequestHandlers_HandleRequest(HttpRequestHandlers *thiz, HttpMessage *request)
{
    HttpMessage * response = NULL;

    do
    {
        response = handleRequest(thiz, request);
        if (response != NULL)
        {
            break;
        }

        response = handleNotFound(thiz, request);
        if (response != NULL)
        {
            break;
        }

        response = HttpMessage_NewHttpResponse(404, "NOT FOUND", NULL, NULL, 0);
    } while (false);

    return response;
}

TINY_LOR
HttpRequestHandlerContext * HttpRequestHandlers_Get(HttpRequestHandlers *thiz, const char *method, const char *uri)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(method, NULL);
    RETURN_VAL_IF_FAIL(uri, NULL);

    for (uint32_t i = 0; i < thiz->size; ++i)
    {
        HttpRequestHandlerContext *item = (HttpRequestHandlerContext *) TinyList_GetAt(thiz, i);

        if (str_equal(item->method, method, true) && uriMatched(item->uri, uri))
        {
            return item;
        }
    }

    return NULL;
}
