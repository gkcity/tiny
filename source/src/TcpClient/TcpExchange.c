/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpExchange.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=454
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "TcpExchange.h"

#define TAG     "TcpExchange"

TINY_LOR
static TinyRet TcpExchange_Construct(TcpExchange *thiz,
                                      const char *ip,
                                      uint16_t port,
                                      uint32_t timeout,
                                      const uint8_t *content,
                                      uint32_t length)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(ip, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(content, TINY_RET_E_ARG_NULL);

    do
    {
        strncpy(thiz->ip, ip, TINY_IP_LEN);
        thiz->port = port;
        thiz->timeout = timeout;

        if (length == 0)
        {
            ret = TINY_RET_E_ARG_INVALID;
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
static void TcpExchange_Dispose(TcpExchange *thiz)
{
    RETURN_IF_FAIL(thiz);

    if (thiz->content != NULL)
    {
        tiny_free(thiz->content);
    }
}

TINY_LOR
TcpExchange * TcpExchange_New(const char *ip,
                              uint16_t port,
                              uint32_t timeout,
                              const uint8_t *content,
                              uint32_t length)
{
    TcpExchange *thiz = NULL;

    do
    {
        thiz = (TcpExchange *)tiny_malloc(sizeof(TcpExchange));
        if (thiz == NULL)
        {
            LOG_D(TAG, "tiny_malloc FAILED");
            break;
        }

        if (RET_FAILED(TcpExchange_Construct(thiz, ip, port, timeout, content, length)))
        {
            TcpExchange_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TINY_LOR
void TcpExchange_Delete(TcpExchange *thiz)
{
    RETURN_IF_FAIL(thiz);

    TcpExchange_Dispose(thiz);
    tiny_free(thiz);
}
