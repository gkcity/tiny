/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpExchange.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=454
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "HttpExchange.h"

#define TAG     "HttpExchange"

TINY_LOR
static TinyRet HttpExchange_Construct(HttpExchange *thiz,
                                      const char *ip,
                                      uint16_t port,
                                      const char *method,
                                      const char *uri,
                                      uint32_t timeout,
                                      const uint8_t *content,
                                      uint32_t length)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(uri, TINY_RET_E_ARG_NULL);

    do
    {
        uint32_t uriLength = 0;

        memset(thiz, 0, sizeof(HttpExchange));
        uriLength = (uint32_t) (strlen(uri) + 1);

        strncpy(thiz->ip, ip, TINY_IP_LEN);
        thiz->port = port;
        strncpy(thiz->method, method, HTTP_METHOD_LEN);

        thiz->uri = tiny_malloc(uriLength);
        if (thiz->uri == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
        memset(thiz->uri, 0, uriLength);
        strncpy(thiz->uri, uri, uriLength);

        ret = HttpHeader_Construct(&thiz->request);
        if (RET_FAILED(ret))
        {
            break;
        }

        thiz->timeout = timeout;

        if (content == NULL || length == 0)
        {
            break;
        }

        thiz->length = length;
        thiz->content = tiny_malloc(length);
        if (thiz->content == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
        memset(thiz->content, 0, length);
        memcpy(thiz->content, content, length);
    } while (false);

    return ret;
}

TINY_LOR
static void HttpExchange_Dispose(HttpExchange *thiz)
{
    RETURN_IF_FAIL(thiz);

    if (thiz->uri != NULL)
    {
        tiny_free(thiz->uri);
    }

    if (thiz->content != NULL)
    {
        tiny_free(thiz->content);
    }
}

TINY_LOR
HttpExchange * HttpExchange_New(const char *ip,
                                uint16_t port,
                                const char *method,
                                const char *uri,
                                uint32_t timeout,
                                const uint8_t *content,
                                uint32_t length)
{
    HttpExchange *thiz = NULL;

    do
    {
        thiz = (HttpExchange *)tiny_malloc(sizeof(HttpExchange));
        if (thiz == NULL)
        {
            LOG_D(TAG, "tiny_malloc FAILED");
            break;
        }

        if (RET_FAILED(HttpExchange_Construct(thiz, ip, port, method, uri, timeout, content, length)))
        {
            HttpExchange_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TINY_LOR
void HttpExchange_Delete(HttpExchange *thiz)
{
    RETURN_IF_FAIL(thiz);

    HttpExchange_Dispose(thiz);
    tiny_free(thiz);
}