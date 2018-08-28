/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WsClient.h
 *
 * @remark
 *
 */

#ifndef __WEB_SOCKET_CLIENT_H__
#define __WEB_SOCKET_CLIENT_H__

#include <tiny_base.h>
#include <bootstrap/Bootstrap.h>
#include "HttpExchange.h"

TINY_BEGIN_DECLS


typedef struct _WsClient
{
    Channel         * client;
    Bootstrap         bootstrap;
} WsClient;

TINY_API
TINY_LOR
WsClient * WsClient_New(void);

TINY_API
TINY_LOR
void WsClient_Delete(WsClient *thiz);

TINY_API
TINY_LOR
TinyRet WsClient_Connect(WsClient *thiz);

TINY_API
TINY_LOR
TinyRet WsClient_Disconnect(WsClient *thiz);

TINY_API
TINY_LOR
TinyRet WsClient_SetHandler(WsClient *thiz);

TINY_API
TINY_LOR
TinyRet WsClient_Send(WsClient *thiz, HttpExchange *exchange);


TINY_END_DECLS

#endif /* __WEB_SOCKET_CLIENT_H__ */