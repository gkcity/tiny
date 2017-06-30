/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_socket_windows.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __TINY_SOCKET_WINDOWS_H__
#define __TINY_SOCKET_WINDOWS_H__

#include <tiny_typedef.h>

TINY_BEGIN_DECLS


#define tiny_select             select
#define tiny_socket_close       closesocket

TINY_INLINE
int tiny_socket_set_block(int fd, bool block)
{
    unsigned long ul = block ? 0 : 1;
    return ioctlsocket(fd, FIONBIO, &ul);
}


TINY_END_DECLS

#endif /* __TINY_SOCKET_WINDOWS_H__ */