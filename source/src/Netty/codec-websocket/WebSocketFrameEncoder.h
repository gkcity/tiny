/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WebSocketFrameEncoder.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#ifndef __WEB_SOCKET_FRAME_ENCODER_H__
#define __WEB_SOCKET_FRAME_ENCODER_H__

#include <tiny_base.h>
#include <tiny_api.h>
#include "WebSocketFrame.h"

TINY_BEGIN_DECLS


typedef void (*WebSocketFrameOutput) (const uint8_t *data, uint64_t size, void *ctx);

TINY_API
TINY_LOR
void WebSocketFrameEncoder_Encode(WebSocketFrame *frame, WebSocketFrameOutput output, void *ctx);



TINY_END_DECLS

#endif /* __WEB_SOCKET_FRAME_ENCODER_H__ */