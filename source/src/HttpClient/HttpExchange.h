/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpExchange.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=454
 *      set expandtab
 */

#ifndef __HTTP_EXCHANGE_H__
#define __HTTP_EXCHANGE_H__

#include <channel/ChannelHandler.h>
#include <codec-http/HttpMessage.h>

TINY_BEGIN_DECLS


typedef struct _HttpExchange
{
    char                      ip[TINY_IP_LEN];
    uint16_t                  port;
    char                      method[HTTP_METHOD_LEN];
    char                    * uri;
    uint32_t                  timeout;
    int                       status;
    uint32_t                  length;
    char                    * content;
} HttpExchange;

TINY_API
TINY_LOR
HttpExchange * HttpExchange_New(const char *ip, uint16_t port, const char *method, const char *uri, uint32_t timeout);

TINY_API
TINY_LOR
void HttpExchange_Delete(HttpExchange *thiz);


TINY_END_DECLS

#endif /* __HTTP_EXCHANGE_H__ */