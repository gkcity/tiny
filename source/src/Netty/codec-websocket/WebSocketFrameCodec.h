/**
 * Copyright (C) 2017-2020
 *
 * @author jxfengzi@gmail.com
 * @date   2017-7-1
 *
 * @file   WebSocketFrameCodec.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __WEB_SOCKET_FRAME_CODEC_H__
#define __WEB_SOCKET_FRAME_CODEC_H__

#include <tiny_base.h>
#include <channel/ChannelHandler.h>


TINY_BEGIN_DECLS


#define WebSocketFrameCodec_Name                "WebSocketFrameCodec"
#define WebSocketFrameCodec_RECV_BUFFER_SIZE    1024
#define WebSocketFrameCodec_SEND_BUFFER_SIZE    1024

TINY_API
TINY_LOR
ChannelHandler * WebSocketFrameCodec(void);


TINY_END_DECLS

#endif /* __WEB_SOCKET_FRAME_CODEC_H__  */