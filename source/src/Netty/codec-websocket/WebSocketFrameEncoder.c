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

#include <tiny_log.h>
#include "WebSocketFrameEncoder.h"

#define TAG     "WebSocketFrameEncoder"

TINY_LOR
void WebSocketFrameEncoder_Encode(WebSocketFrame *frame, WebSocketFrameOutput output, void *ctx)
{
    if (frame->length < 0x7E)
    {
        WebSocketFrameMaxHeader1 header;
        uint32_t length = sizeof(WebSocketFrameMaxHeader1);

        header.header.FIN = (uint16_t) frame->final;
        header.header.RSV1 = 0;
        header.header.RSV2 = 0;
        header.header.RSV3 = 0;
        header.header.Opcode = frame->opcode;
        header.header.MASK = (uint16_t) frame->mask;
        header.header.PayLoadLen = (uint16_t) frame->length;

        if (frame->mask)
        {
            header.maskingKey[0] = frame->maskingKey[0];
            header.maskingKey[1] = frame->maskingKey[1];
            header.maskingKey[2] = frame->maskingKey[2];
            header.maskingKey[3] = frame->maskingKey[3];
            output((uint8_t *) &header, length, ctx);
        }
        else
        {
            output((uint8_t *) &header, length - 4, ctx);
        }
    }
    else if (frame->length < 0xFFFF)
    {
        WebSocketFrameMaxHeader2 header;
        uint32_t length = sizeof(WebSocketFrameMaxHeader2);

        header.header.FIN = (uint16_t) frame->final;
        header.header.RSV1 = 0;
        header.header.RSV2 = 0;
        header.header.RSV3 = 0;
        header.header.Opcode = frame->opcode;
        header.header.MASK = (uint16_t) frame->mask;
        header.header.PayLoadLen = 0x7E;
        header.payloadLength = (uint16_t) frame->length;

        if (frame->mask)
        {
            header.maskingKey[0] = frame->maskingKey[0];
            header.maskingKey[1] = frame->maskingKey[1];
            header.maskingKey[2] = frame->maskingKey[2];
            header.maskingKey[3] = frame->maskingKey[3];
            output((uint8_t *) &header, length, ctx);
        }
        else
        {
            output((uint8_t *) &header, length - 4, ctx);
        }
    }
    else
    {
        WebSocketFrameMaxHeader3 header;
        uint32_t length = sizeof(WebSocketFrameMaxHeader3);

        header.header.FIN = (uint16_t) frame->final;
        header.header.RSV1 = 0;
        header.header.RSV2 = 0;
        header.header.RSV3 = 0;
        header.header.Opcode = frame->opcode;
        header.header.MASK = (uint16_t) frame->mask;
        header.header.PayLoadLen = 0x7F;
        header.payloadLength = frame->length;

        if (frame->mask)
        {
            header.maskingKey[0] = frame->maskingKey[0];
            header.maskingKey[1] = frame->maskingKey[1];
            header.maskingKey[2] = frame->maskingKey[2];
            header.maskingKey[3] = frame->maskingKey[3];
            output((uint8_t *) &header, length, ctx);
        }
        else
        {
            output((uint8_t *) &header, length - 4, ctx);
        }
    };

    if (frame->length != 0)
    {
        output(frame->data, frame->length, ctx);
    }
}