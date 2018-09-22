/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WebSocketFrameFactory.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include "WebSocketFrameFactory.h"

#define TAG     "WebSocketFrameFactory"

TINY_LOR
WebSocketFrame * WebSocketFrameFactory_NewTextFrame(bool final, const char *text)
{
    WebSocketFrame * thiz = WebSocketFrame_New();

    LOG_D(TAG, "WebSocketFrameFactory_NewTextFrame: %s", text);

    if (thiz != NULL)
    {
        thiz->final = final;
        thiz->opcode = OPCODE_TEXT_FRAME;
        thiz->length = (uint32_t) strlen(text);
        thiz->data = tiny_malloc((uint32_t)(thiz->length + 1));
        if (thiz->data != NULL)
        {
            memset(thiz->data, 0, (size_t) (thiz->length + 1));
            memcpy(thiz->data, text, (size_t) (thiz->length));
        }
        else
        {
            WebSocketFrame_Delete(thiz);
            thiz = NULL;
        }
    }

    return thiz;
}

TINY_LOR
WebSocketFrame * WebSocketFrameFactory_NewBinaryFrame(bool final, const uint8_t *binary, uint32_t length)
{
    WebSocketFrame * thiz = WebSocketFrame_New();
    if (thiz != NULL)
    {
        thiz->final = final;
        thiz->opcode = OPCODE_BINARY_FRAME;
        thiz->length = length;
        thiz->data = tiny_malloc((uint32_t)thiz->length);
        if (thiz->data != NULL)
        {
            memcpy(thiz->data, binary, (size_t) (thiz->length));
        }
        else
        {
            WebSocketFrame_Delete(thiz);
            thiz = NULL;
        }
    }

    return thiz;
}

TINY_LOR
WebSocketFrame * WebSocketFrameFactory_NewPingFrame(void)
{
    WebSocketFrame * thiz = WebSocketFrame_New();
    if (thiz != NULL)
    {
        thiz->opcode = OPCODE_PING;
    }

    return thiz;
}

TINY_LOR
WebSocketFrame * WebSocketFrameFactory_NewPongFrame(void)
{
    WebSocketFrame * thiz = WebSocketFrame_New();
    if (thiz != NULL)
    {
        thiz->opcode = OPCODE_PONG;
    }

    return thiz;
}