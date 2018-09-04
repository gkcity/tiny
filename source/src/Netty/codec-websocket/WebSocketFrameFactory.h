/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WebSocketFrameFactory.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __WEB_SOCKET_FRAME_FACTORY_H__
#define __WEB_SOCKET_FRAME_FACTORY_H__

#include <tiny_base.h>
#include "WebSocketFrame.h"

TINY_BEGIN_DECLS


TINY_API
TINY_LOR
WebSocketFrame * WebSocketFrameFactory_NewTextFrame(bool final, const char *text);

TINY_API
TINY_LOR
WebSocketFrame * WebSocketFrameFactory_NewBinaryFrame(bool final, const uint8_t *binary, uint32_t length);

TINY_API
TINY_LOR
WebSocketFrame * WebSocketFrameFactory_NewPingFrame(void);

TINY_API
TINY_LOR
WebSocketFrame * WebSocketFrameFactory_NewPongFrame(void);


TINY_END_DECLS

#endif /* __WEB_SOCKET_FRAME_FACTORY_H__ */