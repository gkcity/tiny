/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpClientHandler.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=454
 *      set expandtab
 */

#ifndef __HTTP_CLIENT_HANDLER_H__
#define __HTTP_CLIENT_HANDLER_H__

#include <channel/ChannelHandler.h>
#include "HttpExchange.h"

TINY_BEGIN_DECLS


#define HttpClientHandler_Name "HttpClientHandler"

TINY_API
TINY_LOR
ChannelHandler * HttpClientHandler(_IN_ HttpExchange *context);


TINY_END_DECLS

#endif /* __HTTP_CLIENT_HANDLER_H__ */