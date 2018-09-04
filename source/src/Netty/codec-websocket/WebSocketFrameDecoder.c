/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WebSocketFrameDecoder.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include <tiny_log.h>
#include <tiny_malloc.h>
#include <tiny_inet.h>
#include "../../ByteBuffer/ByteBuffer.h"
#include "WebSocketFrameEncoder.h"

#define TAG "WebSocketFrameEncoder"

TINY_LOR
WebSocketFrame * WebSocketFrameDecoder_Decode(ByteBuffer *buffer)
{
    WebSocketFrame *frame = NULL;

    RETURN_VAL_IF_FAIL(buffer, NULL);

    do
    {
        WebSocketFrameHeader header;
        uint32_t headerSize = sizeof(WebSocketFrameHeader);
        uint32_t extendedPayloadLength = 0;
        uint32_t maskingKeyLength = 0;
        memset(&header, 0, headerSize);

        if (! ByteBuffer_Pick(buffer, 0, (uint8_t *)&header, headerSize))
        {
            LOG_E(TAG, "ByteBuffer_Pick FAILED, buffer.available: %d", buffer->available);
            break;
        }

        frame = WebSocketFrame_New();
        if (frame == NULL)
        {
            LOG_E(TAG, "WebSocketFrame_New FAILED");
            break;
        }

        frame->final = (header.FIN == 1);
        frame->opcode = (uint8_t) header.Opcode;
        frame->mask = (header.MASK == 1);

        if (header.PayLoadLen < 0x7E)
        {
            frame->length = header.PayLoadLen;
        }
        else if (header.PayLoadLen == 0x7E)
        {
            extendedPayloadLength = 2;
            uint16_t payLoadLen = 0;

            if (! ByteBuffer_Pick(buffer, headerSize, (uint8_t *)&payLoadLen, extendedPayloadLength))
            {
                LOG_E(TAG, "get PayLoadLen FAILED");
                WebSocketFrame_Delete(frame);
                frame = NULL;
                break;
            }

            frame->length = ntohs(payLoadLen);
        }
        else {
            LOG_E(TAG, "not support length: 0x7F");
            WebSocketFrame_Delete(frame);
            frame = NULL;
            break;
        }

        if (frame->mask == 1)
        {
            maskingKeyLength = 4;

            if (! ByteBuffer_Pick(buffer, headerSize + extendedPayloadLength, frame->maskingKey, maskingKeyLength))
            {
                LOG_E(TAG, "get masking key FAILED");
                WebSocketFrame_Delete(frame);
                frame = NULL;
                break;
            }
        }

        LOG_E(TAG, "FIN: %d", frame->final);
        LOG_E(TAG, "OPCODE: %d", frame->opcode);
        LOG_E(TAG, "MASK: %d", frame->mask);
        LOG_E(TAG, "length: %ld", frame->length);

        if (frame->length == 0)
        {
            ByteBuffer_Get(buffer, 0, NULL, headerSize + extendedPayloadLength + maskingKeyLength);
            break;
        }

        frame->data = tiny_malloc((uint32_t)frame->length);
        if (frame->data == NULL)
        {
            LOG_E(TAG, "tiny_malloc failed: %ld", frame->length);
            WebSocketFrame_Delete(frame);
            frame = NULL;
            break;
        }

        if (! ByteBuffer_Pick(buffer, headerSize + extendedPayloadLength + maskingKeyLength, frame->data, (uint32_t) frame->length))
        {
            LOG_E(TAG, "pick data failed: %ld", frame->length);
            WebSocketFrame_Delete(frame);
            frame = NULL;
            break;
        }

        ByteBuffer_Get(buffer, 0, NULL, headerSize + extendedPayloadLength + maskingKeyLength + (uint32_t) frame->length);
    } while (false);

    return frame;
}