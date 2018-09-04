/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WebSocketFrameDecoder.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#ifndef __WEB_SOCKET_FRAME_DECODER_H__
#define __WEB_SOCKET_FRAME_DECODER_H__

#include <tiny_base.h>
#include <tiny_api.h>
#include <buffer/ByteBuffer.h>
#include "WebSocketFrame.h"

TINY_BEGIN_DECLS


TINY_API
TINY_LOR
WebSocketFrame * WebSocketFrameDecoder_Decode(ByteBuffer *buffer);



TINY_END_DECLS

#endif /* __WEB_SOCKET_FRAME_DECODER_H__ */