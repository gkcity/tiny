/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WebSocketFrameEncoder.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include <tiny_log.h>
#include <tiny_malloc.h>
#include "WebSocketFrameEncoder.h"

#define TAG "WebSocketFrameEncoder"

TINY_LOR
WebSocketFrame * WebSocketFrameDecoder_Decode(TinyBuffer *buffer)
{
    WebSocketFrame *frame = NULL;

    RETURN_VAL_IF_FAIL(buffer, NULL);

    do
    {
        WebSocketFrameHeader *header = (WebSocketFrameHeader *)buffer->bytes;
        uint8_t * p = NULL;
        uint64_t unparsed = 0;

        if (buffer->used < sizeof(WebSocketFrameHeader))
        {
            LOG_E(TAG, "invalid buffer length: %d", buffer->used);
            break;
        }

        frame = WebSocketFrame_New();
        if (frame == NULL)
        {
            LOG_E(TAG, "WebSocketFrame_New FAILED");
            break;
        }

        frame->final = (header->FIN == 1);
        frame->opcode = (uint8_t) header->Opcode;
        frame->mask = (header->MASK == 1);

        p = buffer->bytes + sizeof(WebSocketFrameHeader);
        unparsed = buffer->used - sizeof(WebSocketFrameHeader);

        LOG_E(TAG, "unparsed: %ld", unparsed);

        // 111 1111 = 7F = 2^8 -1 = 127
        if (header->PayLoadLen < 0x7E)
        {
            frame->length = header->PayLoadLen;

        }
        else if (header->PayLoadLen == 0x7E)
        {
//            WebSocketFrameMaxHeader2 *h = (WebSocketFrameMaxHeader2 *) buffer->bytes;
//            frame->length = h->payloadLength;

            uint32_t b0 = p[0];
            uint32_t b1 = p[1];
            frame->length = (b1 << 8) + b0;
            p += 2;
        }
        else
        {
//            WebSocketFrameMaxHeader3 *h = (WebSocketFrameMaxHeader3 *) buffer->bytes;
//            frame->length = h->payloadLength;

            uint64_t b0 = p[0];
            uint64_t b1 = p[1];
            uint64_t b2 = p[2];
            uint64_t b3 = p[3];
            uint64_t b4 = p[4];
            uint64_t b5 = p[5];
            uint64_t b6 = p[6];
            uint64_t b7 = p[7];

            frame->length = b0
                            + (b1 << 8)
                            + (b2 << 16)
                            + (b3 << 24)
                            + (b4 << 32)
                            + (b5 << 40)
                            + (b6 << 48)
                            + (b7 << 56);
            p += 8;
        }

        if (header->MASK == 1)
        {
            if (unparsed < 4)
            {
                LOG_E(TAG, "invalid frame");
                WebSocketFrame_Delete(frame);
                frame = NULL;
                break;
            }

            frame->maskingKey[0] = p[0];
            frame->maskingKey[1] = p[1];
            frame->maskingKey[2] = p[2];
            frame->maskingKey[3] = p[3];

            unparsed -= 4;
            p += 4;
        }

        LOG_E(TAG, "FIN: %d", frame->final);
        LOG_E(TAG, "OPCODE: %d", frame->opcode);
        LOG_E(TAG, "MASK: %d", frame->mask);
        LOG_E(TAG, "length: %ld", frame->length);
        LOG_E(TAG, "unparsed: %ld", unparsed);

        if (frame->length != unparsed)
        {
            LOG_E(TAG, "invalid frame, length: %ld, unparsed: %ld", frame->length, unparsed);
            WebSocketFrame_Delete(frame);
            frame = NULL;
            break;
        }

        if (frame->length == 0)
        {
            break;
        }

        frame->data = tiny_malloc(frame->length);
        if (frame->data == NULL)
        {
            LOG_E(TAG, "tiny_malloc failed: %ld", frame->length);
            WebSocketFrame_Delete(frame);
            frame = NULL;
            break;
        }

        memcpy(frame->data, p, unparsed);
    } while (false);

    return frame;
}