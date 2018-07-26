/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpClientHandler.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=454
 *      set expandtab
 */

#ifndef __TCP_CLIENT_HANDLER_H__
#define __TCP_CLIENT_HANDLER_H__

#include <channel/ChannelHandler.h>
#include "TcpExchange.h"

TINY_BEGIN_DECLS


#define TcpClientHandler_Name "TcpClientHandler"

TINY_API
TINY_LOR
ChannelHandler * TcpClientHandler(TcpExchange *context);


TINY_END_DECLS

#endif /* __TCP_CLIENT_HANDLER_H__ */