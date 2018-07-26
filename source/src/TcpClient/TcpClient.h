/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpClient.h
 *
 * @remark
 *
 */

#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include <tiny_base.h>
#include <bootstrap/Bootstrap.h>
#include "TcpExchange.h"

TINY_BEGIN_DECLS


typedef struct _TcpClient
{
    Channel         * client;
    Bootstrap         bootstrap;
} TcpClient;

TINY_API
TINY_LOR
TcpClient * TcpClient_New(void);

TINY_API
TINY_LOR
void TcpClient_Delete(TcpClient *thiz);

TINY_API
TINY_LOR
TinyRet TcpClient_Send(TcpClient *thiz, TcpExchange *exchange);


TINY_END_DECLS

#endif /* __TCP_CLIENT_H__ */