/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   WebSocketFrame.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include "WebSocketFrame.h"


TINY_LOR
WebSocketFrame * WebSocketFrame_New(void)
{
    WebSocketFrame * thiz = (WebSocketFrame *) tiny_malloc(sizeof(WebSocketFrame));
    if (thiz != NULL)
    {
        memset(thiz, 0, sizeof(WebSocketFrame));
        thiz->final = false;
        thiz->mask = false;
        thiz->opcode = 0;
        thiz->length = 0;
        thiz->data = NULL;
    }

    return thiz;
}

TINY_LOR
void WebSocketFrame_Delete(WebSocketFrame *thiz)
{
    if (thiz->data != NULL)
    {
        tiny_free(thiz->data);
    }

    tiny_free(thiz);
}