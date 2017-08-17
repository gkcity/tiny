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
#include <channel/SocketChannel.h>
#include "MulticastChannel.h"
#include "MulticastChannelContext.h"


#define TAG     "MulticastChannel"

static void MulticastChannel_Dispose(Channel *thiz)
{
    SocketChannel_LeaveGroup(thiz);

    if (thiz->ctx != NULL)
    {
        MulticastChannelContext_Delete((MulticastChannelContext *)thiz->ctx);
        thiz->ctx = NULL;
    }
}

static void MulticastChannel_Delete(Channel *thiz)
{
    MulticastChannel_Dispose(thiz);
    tiny_free(thiz);
}

static void MulticastChannel_OnRegister(Channel *thiz, Selector *selector)
{
    if (Channel_IsActive(thiz))
    {
        Selector_Register(selector, thiz->fd, SELECTOR_OP_READ);
        LOG_D(TAG, "MulticastChannel_OnRegister: %d", thiz->fd);

//        MulticastChannelContext *ctx = (MulticastChannelContext *)thiz->ctx;
//        for (int i = 0; i < TinyList_GetCount(&ctx->channels); ++i)
//        {
//            Channel *c = (Channel *) TinyList_GetAt(&ctx->channels, i);
//            Selector_Register(selector, c->fd, SELECTOR_OP_READ);
//        }
    }
}

static void MulticastChannel_OnRemove(Channel *thiz)
{
    LOG_D(TAG, "MulticastChannel_OnRemove");

    MulticastChannel_Delete(thiz);
}

static int _ReadData(Channel *thiz, char buf[], uint32_t buf_len, struct sockaddr_in *remote, socklen_t *socklen)
{
    int received = 0;

    RETURN_VAL_IF_FAIL(thiz, -1);
    RETURN_VAL_IF_FAIL(buf, -1);

    LOG_D(TAG, "_ReadData");

    received = recvfrom(thiz->fd, buf, buf_len, 0, (struct sockaddr *)remote, socklen);
    if (received < 0)
    {
#ifdef _WIN32
        DWORD e = GetLastError();
        LOG_D(TAG, "recvfrom < 0, e = %d", e);
#else
        //LOG_D(TAG, "recvfrom: %s", strerror(errno));
#endif
        return (int)received;
    }
    else if (received == 0)
    {
        return 0;
    }

    return (int) received;
}

static TinyRet MulticastChannel_OnRead(Channel *thiz, Selector *selector)
{
    char buf[1024];
    int bytes_read = 0;
    struct sockaddr_in from;
    socklen_t socklen = (socklen_t) sizeof(from);

    LOG_D(TAG, "MulticastChannel_OnRead");

    if (! Selector_IsReadable(selector, thiz->fd))
    {
        return TINY_RET_OK;
    }

    memset(buf, 0, 1024);

    bytes_read = _ReadData(thiz, buf, 1024, &from, &socklen);
    if (bytes_read <= 0)
    {
        Channel_Close(thiz);
        return TINY_RET_E_INTERNAL;
    }

//    char ip[TINY_IP_LEN];
//    const char *from_ip = NULL;
//    from_ip = inet_ntoa(from.sin_addr);
//    strncpy(ip, from_ip, TINY_IP_LEN);

    LOG_D(TAG, "RECV %d bytes", bytes_read);
    thiz->remote.socket.address = from.sin_addr.s_addr;
    thiz->remote.socket.port = ntohs(from.sin_port);
    SocketChannel_StartRead(thiz, DATA_RAW, buf, (uint32_t) bytes_read);

    return TINY_RET_OK;
}

static void MulticastChannel_OnInactive(Channel *thiz)
{
//    MulticastChannelContext *ctx = (MulticastChannelContext *)thiz->ctx;

    LOG_D(TAG, "MulticastChannel_OnInactive");

//    for (int i = 0; i < TinyList_GetCount(&ctx->channels); ++i)
//    {
//        Channel *channel = (Channel *)TinyList_GetAt(&ctx->channels, i);
//        channel->onInactive(channel);
//        Channel_Close(channel);
//    }
}

static TinyRet MulticastChannel_Construct(Channel *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        thiz->onRegister = MulticastChannel_OnRegister;
        thiz->onRemove = MulticastChannel_OnRemove;
        thiz->onInactive = MulticastChannel_OnInactive;
        thiz->onRead = MulticastChannel_OnRead;

        thiz->ctx = MulticastChannelContext_New();
        if (thiz->ctx == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }
    } while (0);

    return ret;
}

Channel * MulticastChannel_New(void)
{
    Channel * thiz = NULL;

    do
    {
//        thiz = (Channel *)tiny_malloc(sizeof(Channel));
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

TinyRet MulticastChannel_Initialize(Channel *thiz, ChannelInitializer initializer, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(initializer, TINY_RET_E_ARG_NULL);

    initializer(thiz, ctx);

    return TINY_RET_OK;
}

TinyRet MulticastChannel_Join(Channel *thiz, const char *ip, const char *group, uint16_t port)
{
    TinyRet ret = TINY_RET_OK;

    LOG_D(TAG, "MulticastChannel_Join: %s %s:%d", ip, group, port);

    do
    {
        MulticastChannelContext *ctx = (MulticastChannelContext *)thiz->ctx;

        ret = SocketChannel_Open(thiz, TYPE_UDP);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_Open failed");
            break;
        }

        ret = SocketChannel_Bind(thiz, port);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_Bind failed");
            break;
        }

        ret = SocketChannel_JoinGroup(thiz, ip, group);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_JoinGroup failed");
            break;
        }

        ret = SocketChannel_SetBlock(thiz, false);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "SocketChannel_SetBlock failed");
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
    struct sockaddr_in to;
    socklen_t to_len = sizeof(to);

    //return MulticastChannel_WriteTo(thiz, ctx->group, ctx->port, data, len);

    LOG_D(TAG, "MulticastChannel_Write");

    memset(&to, 0, sizeof(to));
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = inet_addr(ctx->group);
    to.sin_port = htons(ctx->port);

    return MulticastChannel_WriteTo(thiz, data, len, (struct sockaddr *)&to, to_len);
}

//TinyRet MulticastChannel_WriteTo(Channel *thiz, const char *ip, uint16_t port, const void *data, uint32_t len)
//{
//    struct sockaddr_in receiver_addr;
//    socklen_t addr_len = sizeof(receiver_addr);
//    ssize_t ret = 0;
//
//    RETURN_VAL_IF_FAIL(thiz, 0);
//    RETURN_VAL_IF_FAIL(ip, 0);
//    RETURN_VAL_IF_FAIL(port, 0);
//    RETURN_VAL_IF_FAIL(data, 0);
//    RETURN_VAL_IF_FAIL(len, 0);
//
//    memset(&receiver_addr, 0, sizeof(receiver_addr));
//    receiver_addr.sin_family = AF_INET;
//    receiver_addr.sin_addr.s_addr = inet_addr(ip);
//    receiver_addr.sin_port = htons(port);
//
//    ret = sendto(thiz->fd, data, len, 0, (struct sockaddr *)&receiver_addr, addr_len);
//    return (ret == len) ? TINY_RET_OK : TINY_RET_E_INTERNAL;
//}

TinyRet MulticastChannel_WriteTo(Channel *thiz, const void *data, uint32_t len, struct sockaddr *to, socklen_t to_len)
{
    int ret = 0;

    RETURN_VAL_IF_FAIL(thiz, 0);
    RETURN_VAL_IF_FAIL(data, 0);
    RETURN_VAL_IF_FAIL(len, 0);
    RETURN_VAL_IF_FAIL(to, 0);
    RETURN_VAL_IF_FAIL(to_len, 0);

    ret = sendto(thiz->fd, data, len, 0, to, to_len);
    return (ret == len) ? TINY_RET_OK : TINY_RET_E_INTERNAL;
}