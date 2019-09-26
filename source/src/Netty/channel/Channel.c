/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   Channel.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_log.h>
#include "Channel.h"

#define TAG     "Channel"

TINY_LOR
void Channel_Close(Channel *thiz)
{
    LOG_I(TAG, "Channel_Close: %s", thiz->id);

    if (thiz->fd != -1)
    {
        tiny_socket_close(thiz->fd);
        thiz->fd = -1;
    }
}

TINY_API
TINY_LOR
void Channel_PreLoop(Channel *thiz)
{
    if (Channel_IsActive(thiz))
    {
        if (thiz->_loopHook != NULL)
        {
            LOG_E(TAG, "call -> Channel.loopHook: %s", thiz->id);
            thiz->_loopHook(thiz, thiz->_loopHookContext);
        }
        else
        {
            LOG_E(TAG, "Channel.loopHook is null: %s", thiz->id);
        }
    }
}
