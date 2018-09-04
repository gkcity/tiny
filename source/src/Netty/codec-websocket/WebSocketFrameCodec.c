/**
 * Copyright (C) 2017-2020
 *
 * @author jxfengzi@gmail.com
 * @date   2017-7-1
 *
 * @file   WebSocketFrameCodec.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <tiny_print_binary.h>
#include <channel/SocketChannel.h>
#include "../../ByteBuffer/ByteBuffer.h"
#include <codec/JsonDecoder.h>
#include "WebSocketFrame.h"
#include "WebSocketFrameCodec.h"
#include "WebSocketFrameEncoder.h"
#include "WebSocketFrameDecoder.h"

#define TAG "WebSocketFrameCodec"

TINY_LOR
static bool _ChannelRead(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    ByteBuffer * buffer = (ByteBuffer *) thiz->context;
    WebSocketFrame * frame = NULL;

    LOG_D(TAG, "_ChannelRead: %d", len);
//    LOG_BINARY(TAG, data, len, true);

    /**
     *
     * 1. put data to context->buffer
     *
     * 2. Decode buffer to a Frame
     *    WebSocketFrame *frame = WebSocketFrame_Decode(buffer);
     *
     * 3. Next
     *    if (frame != NULL)
     *    {
     *         SocketChannel_NextRead(channel, DATA_WEBSOCKET_FRAME, message, 0);
     *    }
     */

    do
    {
        if (! ByteBuffer_Put(buffer, (uint8_t *)data, len))
        {
            LOG_E(TAG, "buffer is full");
            ByteBuffer_Clear(buffer);
            break;
        }

        while (ByteBuffer_Available(buffer))
        {
            frame = WebSocketFrameDecoder_Decode(buffer);
            if (frame == NULL)
            {
                LOG_E(TAG, "WebSocketFrameDecoder_Decode FAILED");
                break;
            }

            SocketChannel_NextRead(channel, DATA_WEBSOCKET_FRAME, frame, 1);
        }
    } while (false);

    if (frame != NULL)
    {
        WebSocketFrame_Delete(frame);
    }

    return true;
}

static void _Output (const uint8_t *data, uint64_t size, void *ctx)
{
    LOG_BINARY(TAG, data, size, true);

    Channel *channel = (Channel *)ctx;
    SocketChannel_StartWrite(channel, DATA_RAW, data, (uint32_t)size);
}

TINY_LOR
static bool _ChannelWrite(ChannelHandler *thiz, Channel *channel, ChannelDataType type, const void *data, uint32_t len)
{
    WebSocketFrame *frame = (WebSocketFrame *) data;

    LOG_D(TAG, "_ChannelWrite");

    do
    {
        if (frame == NULL)
        {
            LOG_E(TAG, "frame is null");
        }

        if (type != thiz->outType)
        {
            LOG_E(TAG, "outType error: %d", type);
            break;
        }

        WebSocketFrameEncoder_Encode(frame, _Output, channel);
    } while (0);

    return true;
}

TINY_LOR
static TinyRet WebSocketFrameCodec_Dispose(ChannelHandler *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    if (thiz->context != NULL)
    {
        ByteBuffer_Delete(thiz->context);
        thiz->context = NULL;
    }

    memset(thiz, 0, sizeof(ChannelHandler));

    return TINY_RET_OK;
}

TINY_LOR
static void WebSocketFrameCodec_Delete(ChannelHandler *thiz)
{
    WebSocketFrameCodec_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
static TinyRet WebSocketFrameCodec_Construct(ChannelHandler *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(ChannelHandler));

        strncpy(thiz->name, WebSocketFrameCodec_Name, CHANNEL_HANDLER_NAME_LEN);

        thiz->invalid = false;
        thiz->onRemove = WebSocketFrameCodec_Delete;
        thiz->inType = DATA_RAW;
        thiz->outType = DATA_WEBSOCKET_FRAME;
        thiz->channelRead = _ChannelRead;
        thiz->channelWrite = _ChannelWrite;

        thiz->context = ByteBuffer_New(WebSocketFrameCodec_RECV_BUFFER_SIZE);
        if (thiz->context == NULL)
        {
            LOG_E(TAG, "TinyBuffer_New FAILED");
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (false);

    return ret;
}

TINY_LOR
ChannelHandler * WebSocketFrameCodec(void)
{
    ChannelHandler *thiz = NULL;

    do
    {
        thiz = (ChannelHandler *)tiny_malloc(sizeof(ChannelHandler));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(WebSocketFrameCodec_Construct(thiz)))
        {
            LOG_E(TAG, "WebSocketFrameCodec_Construct FAILED");
            WebSocketFrameCodec_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}