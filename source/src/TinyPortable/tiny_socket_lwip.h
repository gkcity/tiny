/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_socket_lwip.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __TINY_SOCKET_LWIP_H__
#define __TINY_SOCKET_LWIP_H__

#include <sys/time.h>
#include <lwip/lwip/sockets.h>
#include <tiny_typedef.h>

TINY_BEGIN_DECLS


#define tiny_select             lwip_select
#define tiny_socket_close       lwip_close

int tiny_socket_set_block(int fd, bool block);

TINY_INLINE
int tiny_socket_set_block(int fd, bool block)
{
    int flags = lwip_fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        return flags;
    }

    return lwip_fcntl(fd, F_SETFL, block ? flags & ~O_NONBLOCK : flags | O_NONBLOCK);
}

#define inet_ntop(af,src,dst,size) \
    (((af) == AF_INET) ? ipaddr_ntoa_r((src),(dst),(size)) : NULL)

#define inet_pton(af,src,dst) \
    (((af) == AF_INET) ? inet_aton((src),(dst)) : 0)


TINY_END_DECLS

#endif /* __TINY_SOCKET_LWIP_H__ */