/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   UnicastChannel.c
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
#include "UnicastChannel.h"
#include "UnicastChannelContext.h"


#define TAG             "UnicastChannel"

TINY_LOR
static void UnicastChannel_Dispose(Channel *thiz)
{
    SocketChannel_LeaveGroup(thiz);

    if (thiz->context != NULL)
    {
        UnicastChannelContext_Delete((UnicastChannelContext *)thiz->context);
        thiz->context = NULL;
    }

    SocketChannel_Dispose(thiz);
}

TINY_LOR
static void UnicastChannel_Delete(Channel *thiz)
{
    UnicastChannel_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
static void UnicastChannel_OnRemove(Channel *thiz)
{
    LOG_D(TAG, "UnicastChannel_OnRemove");

    UnicastChannel_Delete(thiz);
}

TINY_LOR
static TinyRet UnicastChannel_OnAccess(Channel *thiz, Selector *selector)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        ByteBuffer *buffer = NULL;
        struct sockaddr_in from;
        socklen_t socklen = (socklen_t) sizeof(from);
        ssize_t received = 0;

        if (! Selector_IsReadable(selector, thiz->fd))
        {
            break;
        }

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
static TinyRet UnicastChannel_Construct(Channel *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        strncpy(thiz->id, "UnicastChannel", CHANNEL_ID_LEN);

//        thiz->_onRegister = UnicastChannel_OnRegister;
        thiz->_onRemove = UnicastChannel_OnRemove;
        thiz->_onAccess = UnicastChannel_OnAccess;

        thiz->_onActive = SocketChannel_OnActive;
        thiz->_onInactive = SocketChannel_OnInactive;

        thiz->context = UnicastChannelContext_New();
        if (thiz->context == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }
    } while (0);

    return ret;
}

TINY_LOR
Channel * UnicastChannel_New(void)
{
    Channel * thiz = NULL;

    do
    {
        thiz = SocketChannel_New();
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(UnicastChannel_Construct(thiz)))
        {
            UnicastChannel_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void UnicastChannel_Initialize(Channel *thiz, ChannelInitializer initializer, void *ctx)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(initializer);

    initializer(thiz, ctx);
}

TINY_LOR
TinyRet UnicastChannel_Open(Channel *thiz, uint16_t port, bool reuse)
{
    TinyRet ret = TINY_RET_OK;

    LOG_I(TAG, "UnicastChannel_Open: %d", port);

    do
    {
        UnicastChannelContext *ctx = (UnicastChannelContext *)thiz->context;

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

        ret = SocketChannel_SetBlock(thiz, false);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "SocketChannel_SetBlock failed: %d", TINY_RET_CODE(ret));
            break;
        }

        ctx->port = port;

        thiz->_onActive(thiz);
    } while (0);

    return ret;
}

TINY_LOR
TinyRet UnicastChannel_WriteTo(Channel *thiz, const void *data, uint32_t len, uint32_t ip, uint16_t port)
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