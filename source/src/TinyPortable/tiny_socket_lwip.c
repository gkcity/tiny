/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_socket_lwip.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include "tiny_socket_lwip.h"

int tiny_socket_set_block(int fd, bool block)
{
    int flags = lwip_fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        return flags;
    }

    return lwip_fcntl(fd, F_SETFL, block ? flags & ~O_NONBLOCK : flags | O_NONBLOCK);
}