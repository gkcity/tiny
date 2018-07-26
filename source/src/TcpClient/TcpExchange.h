/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpExchange.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=454
 *      set expandtab
 */

#ifndef __TCP_EXCHANGE_H__
#define __TCP_EXCHANGE_H__

#include <channel/ChannelHandler.h>

TINY_BEGIN_DECLS


typedef struct _TcpExchange
{
    char                      ip[TINY_IP_LEN];
    uint16_t                  port;
    uint32_t                  timeout;
    char                    * content;
    uint32_t                  length;
} TcpExchange;

TINY_API
TINY_LOR
TcpExchange * TcpExchange_New(const char *ip,
                              uint16_t port,
                              uint32_t timeout,
                              const uint8_t *content,
                              uint32_t length);

TINY_API
TINY_LOR
void TcpExchange_Delete(TcpExchange *thiz);


TINY_END_DECLS

#endif /* __TCP_EXCHANGE_H__ */