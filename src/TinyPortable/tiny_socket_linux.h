/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_socket_linux.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __TINY_SOCKET_LINUX_H__
#define __TINY_SOCKET_LINUX_H__

#include <tiny_typedef.h>

/* According to POSIX.1-2001, POSIX.1-2008 */
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

TINY_BEGIN_DECLS


#define tiny_select                 select
#define tiny_socket_close           close

TINY_INLINE
int tiny_socket_set_block(int fd, bool block)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        return flags;
    }

    return fcntl(fd, F_SETFL, block ? flags & ~O_NONBLOCK : flags | O_NONBLOCK);
}


TINY_END_DECLS

#endif /* __TINY_SOCKET_LINUX_H__ */