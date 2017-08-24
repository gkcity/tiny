/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   MulticastChannel.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <tiny_inet.h>
#include <tiny_socket.h>
#include <channel/SocketChannel.h>
#include "MulticastChannel.h"
#include "MulticastChannelContext.h"


#define TAG             "MulticastChannel"

TINY_LOR
static void MulticastChannel_Dispose(Channel *thiz)
{
    SocketChannel_LeaveGroup(thiz);

    if (thiz->ctx != NULL)
    {
        MulticastChannelContext_Delete((MulticastChannelContext *)thiz->ctx);
        thiz->ctx = NULL;
    }
}

TINY_LOR
static void MulticastChannel_Delete(Channel *thiz)
{
    MulticastChannel_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
static void MulticastChannel_OnRegister(Channel *thiz, Selector *selector)
{
    if (Channel_IsActive(thiz))
    {
        Selector_Register(selector, thiz->fd, SELECTOR_OP_READ);
        LOG_D(TAG, "MulticastChannel_OnRegister: %d", thiz->fd);
    }
}

TINY_LOR
static void MulticastChannel_OnRemove(Channel *thiz)
{
    LOG_D(TAG, "MulticastChannel_OnRemove");

    MulticastChannel_Delete(thiz);
}

TINY_LOR
static TinyRet MulticastChannel_OnReadWrite(Channel *thiz, Selector *selector)
{
    TinyRet ret = TINY_RET_OK;

    LOG_D(TAG, "MulticastChannel_OnRead");

    do {
        char buf[CHANNEL_RECV_BUF_SIZE];
        int received = 0;
        struct sockaddr_in from;
        socklen_t socklen = (socklen_t) sizeof(from);

        if (! Selector_IsReadable(selector, thiz->fd))
        {
            break;
        }

        memset(buf, 0, CHANNEL_RECV_BUF_SIZE);

        received = (int) tiny_recvfrom(thiz->fd, buf, CHANNEL_RECV_BUF_SIZE, 0, (struct sockaddr *)&from, &socklen);
        if (received <= 0)
        {
            Channel_Close(thiz);
            ret = TINY_RET_E_INTERNAL;
            break;
        }

        ChannelAddress_Set(&thiz->remote, TYPE_UDP, from.sin_addr.s_addr, ntohs(from.sin_port));
        SocketChannel_StartRead(thiz, DATA_RAW, buf, (uint32_t) received);
    } while (0);

    return ret;
}

TINY_LOR
static TinyRet MulticastChannel_Construct(Channel *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        strncpy(thiz->id, "MulticastChannel", CHANNEL_ID_LEN);
        printf("%s\n", thiz->id);

        thiz->onRegister = MulticastChannel_OnRegister;
        thiz->onRemove = MulticastChannel_OnRemove;
        thiz->onReadWrite = MulticastChannel_OnReadWrite;

        thiz->onActive = SocketChannel_OnActive;
        thiz->onInactive = SocketChannel_OnInactive;

        thiz->ctx = MulticastChannelContext_New();
        if (thiz->ctx == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }
    } while (0);

    return ret;
}

TINY_LOR
Channel * MulticastChannel_New(void)
{
    Channel * thiz = NULL;

    do
    {
        thiz = SocketChannel_New();
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(MulticastChannel_Construct(thiz)))
        {
            MulticastChannel_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void MulticastChannel_Initialize(Channel *thiz, ChannelInitializer initializer, void *ctx)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(initializer);

    initializer(thiz, ctx);
}

TINY_LOR
TinyRet MulticastChannel_Join(Channel *thiz, const char *ip, const char *group, uint16_t port, bool reuse)
{
    TinyRet ret = TINY_RET_OK;

    printf("MulticastChannel_Join: %s %s:%d\n", ip, group, port);

    do
    {
        MulticastChannelContext *ctx = (MulticastChannelContext *)thiz->ctx;

        ret = SocketChannel_Open(thiz, TYPE_UDP);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "SocketChannel_Open failed: %d", TINY_RET_CODE(ret));
            break;
        }

        ret = SocketChannel_Bind(thiz, port, reuse);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "SocketChannel_Bind failed: %d", TINY_RET_CODE(ret));
            break;
        }

        ret = SocketChannel_JoinGroup(thiz, ip, group);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "SocketChannel_JoinGroup failed: %d", TINY_RET_CODE(ret));
            break;
        }

        ret = SocketChannel_SetBlock(thiz, false);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "SocketChannel_SetBlock failed: %d", TINY_RET_CODE(ret));
            break;
        }

        strncpy(ctx->group, group, TINY_IP_LEN);
        ctx->port = port;
    } while (0);

    return ret;
}

TinyRet MulticastChannel_Write(Channel *thiz, const void *data, uint32_t len)
{
    MulticastChannelContext *ctx = (MulticastChannelContext *)thiz->ctx;
    return MulticastChannel_WriteTo(thiz, data, len, inet_addr(ctx->group), ctx->port);
}

TINY_LOR
TinyRet MulticastChannel_WriteTo(Channel *thiz, const void *data, uint32_t len, uint32_t ip, uint16_t port)
{
    int ret = 0;
    struct sockaddr_in to;
    socklen_t to_len = sizeof(to);

    RETURN_VAL_IF_FAIL(thiz, 0);
    RETURN_VAL_IF_FAIL(data, 0);
    RETURN_VAL_IF_FAIL(len, 0);
    RETURN_VAL_IF_FAIL(ip, 0);
    RETURN_VAL_IF_FAIL(port, 0);

    memset(&to, 0, sizeof(to));
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = ip;
    to.sin_port = htons(port);

    ret = (int) tiny_sendto(thiz->fd, data, len, 0, (struct sockaddr *)&to, (socklen_t)to_len);

    return (ret == len) ? TINY_RET_OK : TINY_RET_E_INTERNAL;
}