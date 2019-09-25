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

    if (thiz->context != NULL)
    {
        MulticastChannelContext_Delete((MulticastChannelContext *)thiz->context);
        thiz->context = NULL;
    }

    SocketChannel_Dispose(thiz);
}

TINY_LOR
static void MulticastChannel_Delete(Channel *thiz)
{
    MulticastChannel_Dispose(thiz);
    tiny_free(thiz);
}

//TINY_LOR
//static void MulticastChannel_OnRegister(Channel *thiz, Selector *selector, ChannelTimer *timer)
//{
//    if (Channel_IsActive(thiz))
//    {
//        Selector_Register(selector, thiz->fd, SELECTOR_OP_READ);
//
//        if (thiz->sendBuffers.size > 0)
//        {
//            Selector_Register(selector, thiz->fd, SELECTOR_OP_WRITE);
//        }
//
//        if (thiz->_getTimeout != NULL)
//        {
//            if (RET_SUCCEEDED(thiz->_getTimeout(thiz, timer, NULL)))
//            {
//                timer->fd = thiz->fd;
//            }
//        }
//    }
//}

TINY_LOR
static void MulticastChannel_OnRemove(Channel *thiz)
{
    LOG_D(TAG, "MulticastChannel_OnRemove");

    MulticastChannel_Delete(thiz);
}

TINY_LOR
static TinyRet MulticastChannel_OnAccess(Channel *thiz, Selector *selector)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        ByteBuffer *buffer = NULL;
        struct sockaddr_in from;
        socklen_t socklen = (socklen_t) sizeof(from);
        int received = 0;

        if (! Selector_IsReadable(selector, thiz->fd))
        {
            break;
        }

//        LOG_I(TAG, "MulticastChannel_OnAccess");

        buffer = ByteBuffer_New(thiz->inBufferSize);
        if (buffer == NULL)
        {
            LOG_I(TAG, "ByteBuffer_New failed!");
            ret = TINY_RET_E_NEW;
            break;
        }

        received = tiny_recvfrom(thiz->fd, buffer->bytes, buffer->size, 0, (struct sockaddr *)&from, &socklen);
        if (received > 0)
        {
            buffer->available = (uint32_t)received;
            ChannelAddress_Set(&thiz->remote, TYPE_UDP, from.sin_addr.s_addr, ntohs(from.sin_port));
            SocketChannel_StartRead(thiz, DATA_RAW, buffer->bytes, (uint32_t) buffer->available);
        }
        else
        {
            LOG_I(TAG, "tiny_recvfrom: %d", buffer->available);

            if (tiny_socket_has_error(thiz->fd))
            {
                ret = TINY_RET_E_SOCKET_READ;
                Channel_Close(thiz);
            }
        }

        ByteBuffer_Delete(buffer);
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

//        thiz->_onRegister = MulticastChannel_OnRegister;
        thiz->_onRemove = MulticastChannel_OnRemove;
        thiz->_onAccess = MulticastChannel_OnAccess;

        thiz->_onActive = SocketChannel_OnActive;
        thiz->_onInactive = SocketChannel_OnInactive;

        thiz->context = MulticastChannelContext_New();
        if (thiz->context == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }
    } while (0);

    return ret;
}

TINY_LOR
Channel * MulticastChannel_New(ChannelLoopHook loopHook, void *ctx)
{
    Channel * thiz = NULL;

    do
    {
        thiz = SocketChannel_New(loopHook, ctx);
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

    LOG_I(TAG, "MulticastChannel_Join: %s %s:%d", ip, group, port);

    do
    {
        MulticastChannelContext *ctx = (MulticastChannelContext *)thiz->context;

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

        thiz->_onActive(thiz);
    } while (0);

    return ret;
}

TINY_LOR
TinyRet MulticastChannel_Write(Channel *thiz, const void *data, uint32_t len)
{
    MulticastChannelContext *ctx = (MulticastChannelContext *)thiz->context;
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