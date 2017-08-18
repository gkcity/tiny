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

TINY_LOR
int tiny_socket_set_block(int fd, bool block)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        return flags;
    }

    return fcntl(fd, F_SETFL, block ? flags & ~O_NONBLOCK : flags | O_NONBLOCK);
}

TINY_LOR
int tiny_socket_join_group(int fd, const char *ip, const char *group)
{
    int ret = 0;

    printf("SocketChannel_JoinGroup: %s group: %s\n", ip, group);

    do
    {
        struct ip_mreq mc;
        unsigned char ttl = 255; // send to any reachable net, not only the subnet
        int ittl = 255;

        mc.imr_multiaddr.s_addr = inet_addr(group);
        mc.imr_interface.s_addr = htonl(INADDR_ANY);
        ret = tiny_setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &mc, sizeof(mc));
        if (ret < 0)
        {
            printf("add membership failed\n");
            break;
        }

        ret = tiny_setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &ttl, sizeof(ttl));
        if (ret < 0)
        {
            printf("set ttl failed\n");
            break;
        }

        ret = tiny_setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &ittl, sizeof(ittl));
        if (ret < 0)
        {
            printf("set multicast ttl failed\n");
            break;
        }

#if 0
        // disable loopback
        loop = 0;
        ret = setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
        if (ret < 0)
        {
            LOG_D(TAG, "disable loopback failed: %s", strerror(errno));
            break;
        }

        // Join the group
        ret = setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&ip, sizeof(ip));
        if (ret < 0)
        {
            LOG_E(TAG, "join multicast interface %s failed: %s", ip, strerror(errno));
            break;
        }
#endif
    } while (0);

    return ret;
}

TINY_LOR
int tiny_socket_leave_group(int fd)
{
//    struct ip_mreq mreq;
//    return setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    return 0;
}