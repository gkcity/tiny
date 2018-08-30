/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WebSocketFrame.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __WEB_SOCKET_FRAME_H__
#define __WEB_SOCKET_FRAME_H__

#include <tiny_base.h>

TINY_BEGIN_DECLS


typedef struct _WebSocketFrameHeader
{
    uint16_t        FIN:1;
    uint16_t        RSV1:1;
    uint16_t        RSV2:1;
    uint16_t        RSV3:1;
    uint16_t        Opcode:4;
    uint16_t        MASK:1;
    uint16_t        PayLoadLen:7;
} WebSocketFrameHeader;

typedef struct _WebSocketFrameMaxHeader1
{
    WebSocketFrameHeader    header;
    uint8_t                 maskingKey[4];
} WebSocketFrameMaxHeader1;

typedef struct _WebSocketFrameMaxHeader2
{
    WebSocketFrameHeader    header;
    uint16_t                payloadLength;
    uint8_t                 maskingKey[4];
} WebSocketFrameMaxHeader2;

typedef struct _WebSocketFrameMaxHeader3
{
    WebSocketFrameHeader    header;
    uint64_t                payloadLength;
    uint8_t                 maskingKey[4];
} WebSocketFrameMaxHeader3;

#define OPCODE_CONTINUATION_FRAME   0x0
#define OPCODE_TEXT_FRAME           0x1
#define OPCODE_BINARY_FRAME         0x2
#define OPCODE_CLOSE_FRAME          0x8
#define OPCODE_PING                 0x9
#define OPCODE_PONG                 0xA


typedef struct _WebSocketFrame
{
    bool        final;
    uint8_t     opcode;
    bool        mask;
    uint8_t     maskingKey[4];
    uint8_t   * data;
    uint64_t    length;
} WebSocketFrame;

TINY_API
TINY_LOR
WebSocketFrame * WebSocketFrame_New(void);

TINY_API
TINY_LOR
void WebSocketFrame_Delete(WebSocketFrame *thiz);


TINY_END_DECLS

#endif /* __WEB_SOCKET_FRAME_H__ */