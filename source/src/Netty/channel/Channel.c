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

#include <tiny_socket.h>
#include "Channel.h"

#define TAG     "Channel"

bool Channel_IsActive(Channel *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return (thiz->fd > 0);
}

bool Channel_IsClosed(Channel *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return (thiz->fd == 0);
}

void Channel_Close(Channel *thiz)
{
    RETURN_IF_FAIL(thiz);

    tiny_socket_close(thiz->fd);

    thiz->fd = 0;
}
