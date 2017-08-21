/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   SocketChannel.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_malloc.h>
#include <tiny_log.h>
#include <tiny_socket.h>
#include <tiny_snprintf.h>

#include "SocketChannel.h"

#define TAG     "SocketChannel"

TINY_LOR
static void _OnChannelRemoved(void * data, void *ctx)
{
    ChannelHandler *handler = (ChannelHandler *)data;
    handler->onRemove(handler);
}

TINY_LOR
static uint16_t _socket_get_port(int fd)
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

TINY_LOR
static TinyRet SocketChannel_Dispose(Channel *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "Channel_Dispose");

    TinyList_Dispose(&thiz->handlers);

    return TINY_RET_OK;
}

TINY_LOR
void SocketChannel_Delete(Channel *thiz)
{
    SocketChannel_Dispose(thiz);
    tiny_free(thiz);
}

TINY_LOR
void SocketChannel_OnRegister(Channel *thiz, Selector *selector)
{
    if (Channel_IsActive(thiz))
    {
        Selector_Register(selector, thiz->fd, SELECTOR_OP_READ);
    }
}

TINY_LOR
void SocketChannel_OnActive(Channel *thiz)
{
    RETURN_IF_FAIL(thiz);

    LOG_D(TAG, "SocketChannel_OnActive, handlers: %d", thiz->handlers.size);

    for (uint32_t i = 0; i < thiz->handlers.size; ++i)
    {
        ChannelHandler *handler = (ChannelHandler *) TinyList_GetAt(&thiz->handlers, i);

        LOG_D(TAG, "ChannelHandler: %s", handler->name);

        if (handler->channelActive != NULL)
        {
            handler->channelActive(handler, thiz);
        }
    }
}

TINY_LOR
void SocketChannel_OnInactive(Channel *thiz)
{
    RETURN_IF_FAIL(thiz);

    for (uint32_t i = 0; i < thiz->handlers.size; ++i)
    {
        ChannelHandler *handler = (ChannelHandler *) TinyList_GetAt(&thiz->handlers, i);
        if (handler->channelInactive != NULL)
        {
            handler->channelInactive(handler, thiz);
        }
    }
}

TINY_LOR
void SocketChannel_OnEventTriggered(Channel *thiz, ChannelTimer *timer)
{
    RETURN_IF_FAIL(thiz);

    for (uint32_t i = 0; i < thiz->handlers.size; ++i)
    {
        ChannelHandler *handler = (ChannelHandler *) TinyList_GetAt(&thiz->handlers, i);
        if (handler->channelEvent != NULL)
        {
            handler->channelEvent(handler, thiz, timer);
        }
    }
}

TINY_LOR
TinyRet SocketChannel_GetTimeout(Channel *thiz, ChannelTimer *timer, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(timer, TINY_RET_E_ARG_NULL);

    for (uint32_t i = 0; i < thiz->handlers.size; ++i)
    {
        ChannelHandler *handler = (ChannelHandler *) TinyList_GetAt(&thiz->handlers, i);
        if (handler->getTimeout != NULL)
        {
            return handler->getTimeout(thiz, timer, handler);
        }
    }

    return TINY_RET_E_NOT_IMPLEMENTED;
}

TINY_LOR
TinyRet SocketChannel_OnReadWrite(Channel *thiz, Selector *selector)
{
    char buf[CHANNEL_RECV_BUF_SIZE];
    int ret = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_MEM(TAG, "OnRead");

    if (! Selector_IsReadable(selector, thiz->fd))
    {
        printf("SocketChannel is not readable: %d\n", thiz->fd);
        return TINY_RET_OK;
    }

    memset(buf, 0, CHANNEL_RECV_BUF_SIZE);

    ret = tiny_recv(thiz->fd, buf, CHANNEL_RECV_BUF_SIZE, 0);
    if (ret > 0)
    {
        SocketChannel_StartRead(thiz, DATA_RAW, buf, (uint32_t) ret);
    }

    return (ret > 0 ) ? TINY_RET_OK : TINY_RET_E_INTERNAL;
}

TINY_LOR
static TinyRet SocketChannel_Construct(Channel *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "Channel_Construct");

    do
    {
        memset(thiz, 0, sizeof(Channel));

        ret = TinyList_Construct(&thiz->handlers);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyList_SetDeleteListener(&thiz->handlers, _OnChannelRemoved, NULL);

        thiz->fd = -1;
        thiz->onRegister = SocketChannel_OnRegister;
        thiz->onReadWrite = SocketChannel_OnReadWrite;
        thiz->onRemove = SocketChannel_Delete;
        thiz->onActive = SocketChannel_OnActive;
        thiz->onInactive = SocketChannel_OnInactive;
        thiz->onEventTriggered = SocketChannel_OnEventTriggered;
        thiz->getTimeout = SocketChannel_GetTimeout;
    } while (0);

    return ret;
}

TINY_LOR
Channel * SocketChannel_New()
{
    Channel *thiz = NULL;

    do
    {
        thiz = (Channel *)tiny_malloc(sizeof(Channel));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(SocketChannel_Construct(thiz)))
        {
            SocketChannel_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TINY_LOR
void SocketChannel_SetRemoteInfo(Channel *thiz, const char *ip, uint16_t port)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(ip);

    tiny_snprintf(thiz->id, CHANNEL_ID_LEN, "%d::%s::%d", thiz->fd, ip, port);

    strncpy(thiz->remote.socket.ip, ip, TINY_IP_LEN);
    thiz->remote.socket.port = port;
}

TINY_LOR
void SocketChannel_Initialize(Channel *thiz, ChannelInitializer initializer, void *ctx)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(initializer);

    initializer(thiz, ctx);
    thiz->onActive(thiz);
}

TINY_LOR
TinyRet SocketChannel_Open(Channel *thiz, ChannelType type)
{
    TinyRet ret = TINY_RET_OK;

	RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        int socket_type = 0;
        int socket_protocol = 0;

        if (type == TYPE_UDP)
        {
            LOG_D(TAG, "SocketChannel_Open: UDP");
            socket_type = SOCK_DGRAM;
//            socket_protocol = IPPROTO_UDP;
            socket_protocol = 0;
        }
        else if (type == TYPE_TCP_SERVER || type == TYPE_TCP_CONNECTION)
        {
            LOG_D(TAG, "SocketChannel_Open: TCP");
            socket_type = SOCK_STREAM;
//            socket_protocol = IPPROTO_TCP;
            socket_protocol = 0;
        }
        else
        {
            LOG_E(TAG, "ChannelType invalid");
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        thiz->fd = tiny_socket(AF_INET, socket_type, socket_protocol);
        if (thiz->fd < 0)
        {
            LOG_E(TAG, "socket failed");
            ret = TINY_RET_E_SOCKET_FD;
            break;
        }
    } while (0);

    return ret;
}

TINY_LOR
TinyRet SocketChannel_Bind(Channel *thiz, uint16_t port, bool reuse)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "SocketChannel_Bind: %d", port);

    printf("SocketChannel_Bind: %d\n", port);

    do
    {
        int value = 0;
        struct sockaddr_in  self_addr;
        memset(&self_addr, 0, sizeof(self_addr));
        self_addr.sin_family = AF_INET;
        self_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        self_addr.sin_port = htons(port);

        if (reuse)
        {
            int err = 1;

            value = tiny_setsockopt(thiz->fd, SOL_SOCKET, SO_REUSEPORT, (char *)&err, sizeof(err));
            if (value < 0)
            {
                //LOG_D(TAG, "setsockopt: %s", strerror(errno));
                ret = TINY_RET_E_SOCKET_SETSOCKOPT;
                break;
            }

            value = tiny_setsockopt(thiz->fd, SOL_SOCKET, SO_REUSEADDR, &err, sizeof(err));
            if (value < 0)
            {
                //LOG_D(TAG, "setsockopt: %s", strerror(errno));
                ret = TINY_RET_E_SOCKET_SETSOCKOPT;
                break;
            }
        }

        value = tiny_bind(thiz->fd, (struct sockaddr *)&self_addr, sizeof(self_addr));
        if (value < 0)
        {
            LOG_D(TAG, "tiny_bind failed: %s", strerror(errno));
            ret = TINY_RET_E_SOCKET_BIND;
            break;
        }

        printf("SocketChannel_Bind OK, port: %d \n", port);

        thiz->local.socket.address = self_addr.sin_addr.s_addr;
        thiz->local.socket.port = port;

        tiny_snprintf(thiz->id, CHANNEL_ID_LEN, "%d::127.0.0.1::%d", thiz->fd,port);
    } while (0);

    return ret;
}

TINY_LOR
TinyRet SocketChannel_SetBlock(Channel *thiz, bool block)
{
    return (tiny_socket_set_block(thiz->fd, block) == 0) ? TINY_RET_OK : TINY_RET_E_INTERNAL;
}

TINY_LOR
TinyRet SocketChannel_Listen(Channel *thiz, int maxConnections)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL((maxConnections > 0), TINY_RET_E_ARG_NULL);

    if (tiny_listen(thiz->fd, maxConnections) < 0)
    {
        return TINY_RET_E_INTERNAL;
    }

    if (thiz->local.socket.port == 0)
    {
        thiz->local.socket.port = _socket_get_port(thiz->fd);
    }

    return TINY_RET_OK;
}

TINY_LOR
TinyRet SocketChannel_JoinGroup(Channel *thiz, const char *ip, const char *group)
{
    return (tiny_socket_join_group(thiz->fd, ip, group) == 0 ? TINY_RET_OK : TINY_RET_E_SOCKET_SETSOCKOPT);
}

TINY_LOR
TinyRet SocketChannel_LeaveGroup(Channel *thiz)
{
    return (tiny_socket_leave_group(thiz->fd) == 0 ? TINY_RET_OK : TINY_RET_E_SOCKET_SETSOCKOPT);
}

TINY_LOR
TinyRet SocketChannel_AddBefore(Channel *thiz, const char *name, ChannelHandler *handler)
{
    int position = -1;

	RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
	RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);
	RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    for (uint32_t i = 0; i < thiz->handlers.size; ++i)
    {
        ChannelHandler *h = (ChannelHandler *) TinyList_GetAt(&thiz->handlers, i);
        if (STR_EQUAL(h->name, name))
        {
            position = i;
            break;
        }
    }

    if (position < 0)
    {
        return TINY_RET_E_NOT_FOUND;
    }

    return TinyList_InsertBefore(&thiz->handlers, position, handler);
}

TINY_LOR
void SocketChannel_AddLast(Channel *thiz, ChannelHandler *handler)
{
	RETURN_IF_FAIL(thiz);
	RETURN_IF_FAIL(handler);

    TinyList_AddTail(&thiz->handlers, handler);
}

TINY_LOR
void SocketChannel_StartRead(Channel *thiz, ChannelDataType type, const void *data, uint32_t len)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(data);
    RETURN_IF_FAIL(len);

    thiz->currentReader = 0;
    SocketChannel_NextRead(thiz, type, data, len);
}

TINY_LOR
void SocketChannel_NextRead(Channel *thiz, ChannelDataType type, const void *data, uint32_t len)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(data);
    RETURN_IF_FAIL(len);

    while (true)
    {
        ChannelHandler *handler = TinyList_GetAt(&thiz->handlers, thiz->currentReader);
        if (handler == NULL)
        {
            // LOG_D(TAG, "ChannelHandler not found: %d", thiz->currentReader);
            break;
        }

        LOG_D(TAG, "ChannelHandler: %s", handler->name);

        if (handler->inType != type)
        {
            LOG_E(TAG, "ChannelDataType not matched: %d != %d", type, handler->inType);
            break;
        }

        thiz->currentReader++;

        if (handler->channelRead == NULL)
        {
            LOG_D(TAG, "%s.channelRead not implemented", handler->name);
            continue;
        }

        if (handler->channelRead(handler, thiz, type, data, len))
        {
            break;
        }
    }
}

TINY_LOR
void SocketChannel_StartWrite(Channel *thiz, ChannelDataType type, const void *data, uint32_t len)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(data);
    RETURN_IF_FAIL(len);

//    printf("SocketChannel_StartWrite: \n%s\n", (const char *)data);

    thiz->currentWriter = thiz->handlers.size - 1;
    SocketChannel_NextWrite(thiz, type, data, len);
}

TINY_LOR
void SocketChannel_NextWrite(Channel *thiz, ChannelDataType type, const void *data, uint32_t len)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(data);
    RETURN_IF_FAIL(len);

    while (true)
    {
        ChannelHandler *handler = TinyList_GetAt(&thiz->handlers, thiz->currentWriter);
        if (handler == NULL)
        {
            LOG_D(TAG, "tiny_send: %d", len);
            int sent = tiny_send(thiz->fd, data, len, 0);
            if (sent != len)
            {
                Channel_Close(thiz);
            }

            break;
        }

        //    if (handler->inType != type)
        //    {
        //        LOG_E(TAG, "ChannelDataType not matched: %d != %d", type, handler->inType);
        //        return ret;
        //    }

        thiz->currentWriter--;

        if (handler->channelWrite == NULL)
        {
            LOG_D(TAG, "%s.channelWrite NOT IMPLEMENTED", handler->name);
            continue;
        }

        LOG_D(TAG, "%s.channelWrite", handler->name);
        if (handler->channelWrite(handler, thiz, handler->inType, data, len))
        {
            break;
        }
    }
}