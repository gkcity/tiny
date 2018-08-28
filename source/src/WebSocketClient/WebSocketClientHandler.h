/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WsClientHandler.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=454
 *      set expandtab
 */

#ifndef __WEB_SOCKET_CLIENT_HANDLER_H__
#define __WEB_SOCKET_CLIENT_HANDLER_H__

#include <channel/ChannelHandler.h>
#include "HttpExchange.h"

TINY_BEGIN_DECLS


#define WsClientHandler_Name "WsClientHandler"

TINY_API
TINY_LOR
ChannelHandler * WsClientHandler(HttpExchange *context);


TINY_END_DECLS

#endif /* __WEB_SOCKET_CLIENT_HANDLER_H__ */