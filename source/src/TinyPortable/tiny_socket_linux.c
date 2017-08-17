/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_socket_linux.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include "tiny_socket_linux.h"

int tiny_socket_set_block(int fd, bool block)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        return flags;
    }

    return fcntl(fd, F_SETFL, block ? flags & ~O_NONBLOCK : flags | O_NONBLOCK);
}