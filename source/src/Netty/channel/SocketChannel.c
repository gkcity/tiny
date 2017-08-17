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
#include <tiny_inet.h>
#include <tiny_snprintf.h>
#include <errno.h>

#include "SocketChannel.h"

#define TAG     "SocketChannel"

static void _OnHandlerRemoved(void * data, void *ctx)
{
    ChannelHandler *handler = (ChannelHandler *)data;
    handler->onRemove(handler);
}

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

static int _ReadData(Channel *thiz, char buf[], uint32_t len)
{
    uint32_t received = 0;

	RETURN_VAL_IF_FAIL(thiz, -1);
	RETURN_VAL_IF_FAIL(buf, -1);

    LOG_D(TAG, "_ReadData");

    while (len > 0)
    {
#ifdef _WIN32
        int n = recv(thiz->fd, buf + received, len , 0);
#else
		ssize_t n = recv(thiz->fd, buf + received, len, 0);
#endif
        LOG_D(TAG, "recv: %ld", n);

        if (n == 0)
        {
            break;
        }

#ifdef _WIN32
        if (n == SOCKET_ERROR)
        {
            break;
        #if 0
            DWORD e = GetLastError();
            LOGI("GetLastError: %d", e);

            if (e == WSAEWOULDBLOCK)
            {
                break;
                air_usleep(100);
                continue;
            }
            else
            {
                break;
            }
        #endif
        }
#else
        if (n == -1)
        {
            break;

            // more data
//            if (errno == EAGAIN)
//            {
//                continue;
//            }
//            else
//            {
//                break;
//            }

        }
#endif

        received += n;
        len -= n;
        break;
    }

    return (received == 0) ? -1 : (int) received;
}

static int _WriteData(Channel *thiz, const char *buf, uint32_t len)
{
    int sent = 0;

	RETURN_VAL_IF_FAIL(thiz, -1);
	RETURN_VAL_IF_FAIL(buf, -1);

    LOG_D(TAG, "_WriteData: buf size is: %u", len);

    while (len > 0)
    {
#ifdef _WIN32
		int n = send(thiz->fd, buf + sent, len, 0);
#else
		ssize_t n = send(thiz->fd, buf + sent, len, 0);
#endif
        if (n == 0)
        {
            break;
        }

        LOG_D(TAG, "send: %ld bytes", n);

#ifdef _WIN32
        if (n == SOCKET_ERROR)
        {
            DWORD e = GetLastError();
            LOG_I(TAG, "GetLastError: %d", e);

            if (e == WSAEWOULDBLOCK)
            {
                //usleep(100);
                continue;
            }
            else
            {
                break;
            }
        }
#else
        if (n == -1)
        {
            break;

            // more data
//            if (errno == EAGAIN)
//            {
//                continue;
//            }
//            else
//            {
//                break;
//            }
        }
#endif

        sent += n;
        len -= n;
    }

    LOG_D(TAG, "_WriteData: %d bytes is sent.", sent);

    return sent;
}

static TinyRet SocketChannel_Dispose(Channel *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "Channel_Dispose");

    TinyList_Dispose(&thiz->handlers);

    return TINY_RET_OK;
}

static void SocketChannel_Delete(Channel *thiz)
{
    SocketChannel_Dispose(thiz);
    tiny_free(thiz);
}

static void SocketChannel_OnRegister(Channel *thiz, Selector *selector)
{
    Selector_Register(selector, thiz->fd, SELECTOR_OP_READ);
}

void SocketChannel_OnRemove(Channel *thiz)
{
    SocketChannel_Delete(thiz);
}

void SocketChannel_OnActive(Channel *thiz)
{
    RETURN_IF_FAIL(thiz);

    for (int i = 0; i < TinyList_GetCount(&thiz->handlers); ++i)
    {
        ChannelHandler *handler = (ChannelHandler *) TinyList_GetAt(&thiz->handlers, i);
        if (handler->channelActive != NULL)
        {
            handler->channelActive(handler, thiz);
        }
    }
}

static void SocketChannel_OnInactive(Channel *thiz)
{
    RETURN_IF_FAIL(thiz);

    for (int i = 0; i < TinyList_GetCount(&thiz->handlers); ++i)
    {
        ChannelHandler *handler = (ChannelHandler *) TinyList_GetAt(&thiz->handlers, i);
        if (handler->channelInactive != NULL)
        {
            handler->channelInactive(handler, thiz);
        }
    }
}

static void SocketChannel_OnEventTriggered(Channel *thiz, void *event)
{
    RETURN_IF_FAIL(thiz);

    for (int i = 0; i < TinyList_GetCount(&thiz->handlers); ++i)
    {
        ChannelHandler *handler = (ChannelHandler *) TinyList_GetAt(&thiz->handlers, i);
        if (handler->channelEvent != NULL)
        {
            handler->channelEvent(handler, thiz, event);
        }
    }
}

static int64_t SocketChannel_NextTimeout(Channel *thiz, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    for (int i = 0; i < TinyList_GetCount(&thiz->handlers); ++i)
    {
        ChannelHandler *handler = (ChannelHandler *) TinyList_GetAt(&thiz->handlers, i);
        if (handler->nextTimeout != NULL)
        {
            return handler->nextTimeout(thiz, handler);
        }
    }

    return 0;
}

TinyRet SocketChannel_OnRead(Channel *thiz, Selector *selector)
{
    char buf[1024];
    int ret = 0;
    int error;
    socklen_t len = sizeof (error);

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    if (! Selector_IsReadable(selector, thiz->fd))
    {
        return TINY_RET_OK;
    }

    memset(buf, 0, 1024);

    ret = _ReadData(thiz, buf, 1024);
    if (ret > 0)
    {
#if 0
        LOG_D(TAG, "read from (%d) %s:%d, ret: %d", thiz->fd, thiz->ip, thiz->port, ret);
        LOG_D(TAG, "%s", buf);
#endif

        SocketChannel_StartRead(thiz, DATA_RAW, buf, (uint32_t) ret);
    }

    return (ret > 0 ) ? TINY_RET_OK : TINY_RET_E_INTERNAL;
}

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

        TinyList_SetDeleteListener(&thiz->handlers, _OnHandlerRemoved, NULL);

        thiz->onRegister = SocketChannel_OnRegister;
        thiz->onRemove = SocketChannel_OnRemove;
        thiz->onRead = SocketChannel_OnRead;
        thiz->onActive = SocketChannel_OnActive;
        thiz->onInactive = SocketChannel_OnInactive;
        thiz->onEventTriggered = SocketChannel_OnEventTriggered;
        thiz->getNextTimeout = SocketChannel_NextTimeout;
    } while (0);

    return ret;
}

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

void SocketChannel_Initialize(Channel *thiz, const char *ip, int fd, uint16_t port, ChannelInitializer initializer, void *ctx)
{
    RETURN_IF_FAIL(thiz);
	RETURN_IF_FAIL(ip);

    tiny_snprintf(thiz->id, CHANNEL_ID_LEN, "%d::%s::%d", fd, ip, port);
    thiz->fd = fd;

    strncpy(thiz->local.socket.ip, ip, IP_LEN);
    thiz->local.socket.port = port;

    initializer(thiz, ctx);

    thiz->onActive(thiz);
}

void SocketChannel_InitializeWithRemoteInfo(Channel *thiz, const char *ip, int fd, uint16_t port, ChannelInitializer initializer, void *ctx)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(ip);

    tiny_snprintf(thiz->id, CHANNEL_ID_LEN, "%d::%s::%d", fd, ip, port);
    thiz->fd = fd;

    strncpy(thiz->remote.socket.ip, ip, IP_LEN);
    thiz->remote.socket.port = port;

    initializer(thiz, ctx);

    thiz->onActive(thiz);
}

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
            socket_protocol = IPPROTO_TCP;
        }
        else
        {
            LOG_E(TAG, "ChannelType invalid");
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        thiz->fd = socket(AF_INET, socket_type, socket_protocol);
        if (thiz->fd < 0)
        {
            LOG_E(TAG, "socket failed");
            ret = TINY_RET_E_INTERNAL;
            break;
        }
        LOG_D(TAG, "socket opened %d", thiz->fd);
    } while (0);

    return ret;
}

TinyRet SocketChannel_Bind(Channel *thiz, uint16_t port)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "SocketChannel_Bind: %d", port);

    do
    {
        int err = 1;
        struct sockaddr_in  self_addr;
        memset(&self_addr, 0, sizeof(self_addr));
        self_addr.sin_family = AF_INET;
        self_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        self_addr.sin_port = htons(port);
#ifndef WIN32
        ret = setsockopt(thiz->fd, SOL_SOCKET, SO_REUSEPORT, (char *)&err, sizeof(err));
        if (ret < 0)
        {
            //LOG_D(TAG, "setsockopt: %s", strerror(errno));
            return TINY_RET_E_SOCKET_SETSOCKOPT;
        }
#endif
        ret = setsockopt(thiz->fd, SOL_SOCKET, SO_REUSEADDR, &err, sizeof(err));
        if (ret < 0)
        {
            //LOG_D(TAG, "setsockopt: %s", strerror(errno));
            return TINY_RET_E_SOCKET_SETSOCKOPT;
        }

        err = bind(thiz->fd, (struct sockaddr *)&self_addr, sizeof(self_addr));

#ifdef _WIN32
        if (err == SOCKET_ERROR)
            {
                LOG_E(TAG, "bind failed");
                ret = TINY_RET_E_INTERNAL;
                break;
            }
#else
        if (err < 0)
        {
            LOG_E(TAG, "bind failed");
            ret = TINY_RET_E_INTERNAL;
            break;
        }
#endif

        thiz->local.socket.address = self_addr.sin_addr.s_addr;
        thiz->local.socket.port = port;
    } while (0);

    return ret;
}

TinyRet SocketChannel_Connect(Channel *thiz, const char *ip, uint16_t port)
{
	TinyRet ret = TINY_RET_OK;
	int err = 0;
	int err2 = 0;
	RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
	LOG_D(TAG, "SocketChannel_Connect: %s:%d", ip, port);
	
	do
	{
		struct sockaddr_in addr;
    	memset(&addr, 0, sizeof (addr));
   		addr.sin_family = AF_INET;
    	addr.sin_port = htons(port);
    	addr.sin_addr.s_addr = inet_addr(ip);
		err = connect(thiz->fd, (struct sockaddr*)&addr, sizeof(struct sockaddr));
#ifdef WIN32
		if (err == SOCKET_ERROR)
#else
		if (err < 0)
#endif
		{
#ifdef WIN32
			err2 = WSAGetLastError();
			if (err2 != WSAEWOULDBLOCK)
#else
			err2 = errno;
			if (errno  != EINPROGRESS)
#endif
			{
				LOG_E(TAG, "connect failed!%d %s", thiz->fd, strerror(err2));
				ret = TINY_RET_E_INTERNAL;
				break;
			} 
			else
			{
				LOG_I(TAG, "connect in progress!%d %s", thiz->fd, strerror(errno));
				ret = TINY_RET_E_SOCKET_CONNECTING;
				break;
			}
		}
		else
		{
			LOG_I(TAG, "connected immidiatelly!");
		}
	} while(0);
	return ret;
}

TinyRet SocketChannel_SetBlock(Channel *thiz, bool block)
{
    return (tiny_socket_set_block(thiz->fd, block) == 0) ? TINY_RET_OK : TINY_RET_E_INTERNAL;
}

TinyRet SocketChannel_Listen(Channel *thiz, int maxConnections)
{
    int ret = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL((maxConnections > 0), TINY_RET_E_ARG_NULL);

    ret = listen(thiz->fd, maxConnections);

#ifdef _WIN32
    if (ret == SOCKET_ERROR)
    {
        return TINY_RET_E_INTERNAL;
    }
#else
    if (ret < 0)
    {
        return TINY_RET_E_INTERNAL;
    }
#endif

    if (thiz->local.socket.port == 0)
    {
        thiz->local.socket.port = _socket_get_port(thiz->fd);
    }

    return TINY_RET_OK;
}

#ifdef _WIN32
TinyRet SocketChannel_JoinGroup(Channel *thiz, const char *ip, const char *group)
{
    struct ip_mreq ipMreqV4;
    struct sockaddr_in addr;
    int ret = NO_ERROR;

    int loop = 1;
    ret = setsockopt(thiz->fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if (ret == SOCKET_ERROR)
    {
        DWORD e = GetLastError();
        LOG_D(TAG, "setsockopt: %d", e);
        return TINY_RET_E_SOCKET_SETSOCKOPT;
    }

    // Setup the v4 option values and ip_mreq structure
    memset(&ipMreqV4, 0, sizeof(struct ip_mreq));
    ipMreqV4.imr_multiaddr.s_addr = inet_addr(group);
    //ipMreqV4.imr_interface.s_addr = htonl(ip);
    ipMreqV4.imr_interface.s_addr = ip;

    // Join the group
    ret = setsockopt(thiz->fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ipMreqV4, sizeof(ipMreqV4));
    if (ret == SOCKET_ERROR)
    {
        // !!! bug 10065 = A socket operation was attempted to an unreachable host.
        DWORD e = GetLastError();
        LOG_E(TAG, "setsockopt: %d", e);
        return TINY_RET_E_SOCKET_SETSOCKOPT;
    }

    return TINY_RET_OK;
}
#else // Linux | unix
TinyRet SocketChannel_JoinGroup(Channel *thiz, const char *ip, const char *group)
{
    struct ip_mreq mc;
    int ret = 0;
//    int loop = 1;
    unsigned char ttl = 255; // send to any reachable net, not only the subnet
    int ittl = 255;

    LOG_D(TAG, "SocketChannel_JoinGroup: %s group: %s", ip, group);

    mc.imr_multiaddr.s_addr = inet_addr(group);
    mc.imr_interface.s_addr = htonl(INADDR_ANY);
    ret = setsockopt(thiz->fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &mc, sizeof(mc));
    if (ret < 0)
    {
        //LOG_D(TAG, "add membership failed: %s", strerror(errno));
        return TINY_RET_E_SOCKET_SETSOCKOPT;
    }

    ret = setsockopt(thiz->fd, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &ttl, sizeof(ttl));
    if (ret < 0)
    {
        //LOG_D(TAG, "set ttl failed: %s", strerror(errno));
        return TINY_RET_E_SOCKET_SETSOCKOPT;
    }

    ret = setsockopt(thiz->fd, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &ittl, sizeof(ittl));
    if (ret < 0)
    {
        //LOG_D(TAG, "set multicast ttl failed: %s", strerror(errno));
        return TINY_RET_E_SOCKET_SETSOCKOPT;
    }

#if 0
    // disable loopback
    loop = 0;
    ret = setsockopt(thiz->fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if (ret < 0)
    {
        LOG_D(TAG, "disable loopback failed: %s", strerror(errno));
        return TINY_RET_E_SOCKET_SETSOCKOPT;
    }

    // Join the group
    ret = setsockopt(thiz->fd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&ip, sizeof(ip));
    if (ret < 0)
    {
        LOG_E(TAG, "join multicast interface %s failed: %s", ip, strerror(errno));
        return TINY_RET_E_SOCKET_SETSOCKOPT;
    }
#endif

    return TINY_RET_OK;
}
#endif // _WIN32

TinyRet SocketChannel_LeaveGroup(Channel *thiz)
{
    LOG_W(TAG, "SocketChannel_LeaveGroup: not implemented");

#if 0
    int ret = 0;
    struct ip_mreq mreq;
    ret = setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
#endif

    return TINY_RET_OK;
}

TinyRet SocketChannel_AddBefore(Channel *thiz, const char *name, ChannelHandler *handler)
{
    int position = -1;

	RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
	RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);
	RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    for (int i = 0; i < TinyList_GetCount(&thiz->handlers); ++i)
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

void SocketChannel_AddLast(Channel *thiz, ChannelHandler *handler)
{
	RETURN_IF_FAIL(thiz);
	RETURN_IF_FAIL(handler);

    TinyList_AddTail(&thiz->handlers, handler);
}

void SocketChannel_StartRead(Channel *thiz, ChannelDataType type, void *data, uint32_t len)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(data);
    RETURN_IF_FAIL(len);

    LOG_D(TAG, "SocketChannel_StartRead");

    thiz->currentReader = 0;
    SocketChannel_NextRead(thiz, type, data, len);
}

void SocketChannel_NextRead(Channel *thiz, ChannelDataType type, void *data, uint32_t len)
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

        LOG_D(TAG, "%s.channelRead", handler->name);
        if (handler->channelRead(handler, thiz, type, data, len))
        {
            break;
        }
    }
}

void SocketChannel_StartWrite(Channel *thiz, ChannelDataType type, const void *data, uint32_t len)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(data);
    RETURN_IF_FAIL(len);

    LOG_D(TAG, "Channel_ChannelWrite");

    thiz->currentWriter = (TinyList_GetCount(&thiz->handlers)) - 1;
    SocketChannel_NextWrite(thiz, type, data, len);
}

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
            //LOG_D(TAG, "ChannelHandler not found: %d", thiz->currentReader);
            int sent = _WriteData(thiz, data, len);
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
