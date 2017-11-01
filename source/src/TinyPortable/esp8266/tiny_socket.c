/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_socket.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include "tiny_socket.h"
#include "tiny_log.h"
#include "../../TinyPorting/tiny_sleep.h"

#define TAG		"tiny_socket"

TINY_LOR
int tiny_socket_set_block(int fd, bool block)
{
    int flags = lwip_fcntl(fd, F_GETFL, 0);
    if (flags < 0)
    {
        return flags;
    }

    return lwip_fcntl(fd, F_SETFL, block ? flags & ~O_NONBLOCK : flags | O_NONBLOCK);
}

TINY_LOR
int tiny_socket_join_group(int fd, const char *ip, const char *group)
{
    int ret = 0;

    LOG_D(TAG, "tiny_socket_join_group: %s group: %s", ip, group);

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
            LOG_E(TAG, "add membership failed\n");
            break;
        }

        ret = tiny_setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &ttl, sizeof(ttl));
        if (ret < 0)
        {
            LOG_E(TAG, "set ttl failed\n");
            break;
        }

        ret = tiny_setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &ittl, sizeof(ittl));
        if (ret < 0)
        {
            LOG_E(TAG, "set multicast ttl failed\n");
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

TINY_LOR
TinyRet tiny_async_connect(int fd, const char *ip, uint16_t port)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        int result = 0;
        struct sockaddr_in remote;
        memset(&remote, 0, sizeof(struct sockaddr_in));
        remote.sin_family = AF_INET;
        remote.sin_addr.s_addr = inet_addr(ip);
        remote.sin_port = htons(port);

        result = tiny_connect(fd, (const struct sockaddr *) &remote, sizeof(struct sockaddr_in));
        if (result < 0)
        {
            ret = (errno == EINPROGRESS) ? TINY_RET_PENDING : TINY_RET_E_SOCKET_CONNECTING;
        }
    } while (0);

    return ret;
}

TINY_LOR
bool tiny_socket_has_error(int fd)
{
    bool error = false;

#if 0
    do
    {
        int error = 0;
        socklen_t len = sizeof(error);

        // BUG!!! ESP8266 will be crashed
        if (tiny_getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
        {
            LOG_E(TAG, "tiny_socket_has_error -> tiny_getsockopt failed.");
            error = true;
            break;
        }

        if (error == 0)
        {
            break;
        }

        LOG_E(TAG, "tiny_socket_has_error: %d", error);

        if (error == EAGAIN)
        {
            LOG_I(TAG, "ignore error: %d (EAGAIN)", error);
            tiny_sleep(10 * 3);
            break;
        }

        error = true;
    } while (false);
#endif

    return error;
}

TINY_LOR
int tiny_socket_reuse_port(int fd)
{
    int ret = 0;
    int reuseport = 1;

    ret = tiny_setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, (char *)&reuseport, sizeof(reuseport));
    if (ret < 0)
    {
        LOG_E(TAG, "tiny_setsockopt failed: %d", ret);
    }

    LOG_D(TAG, "tiny_socket_reuse_port, reuseport = %d", reuseport);

    return ret;
}

TINY_LOR
int tiny_socket_reuse_address(int fd)
{
    int ret = 0;
    int reuseaddr = 1;

    ret = tiny_setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
    if (ret < 0)
    {
        LOG_E(TAG, "tiny_setsockopt failed: %d", ret);
    }

    LOG_D(TAG, "tiny_socket_reuse_address, reuseaddr = %d", reuseaddr);

    return ret;
}

TINY_LOR
uint16_t tiny_socket_get_port(int fd)
{
    uint16_t port = 0;
    struct sockaddr_in sin;
    socklen_t len = (socklen_t) sizeof(sin);

    if (getsockname(fd, (struct sockaddr *)&sin, &len) == 0)
    {
        port = ntohs(sin.sin_port);
    }

    return port;
}