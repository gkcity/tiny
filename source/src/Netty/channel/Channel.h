/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   Channel.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <tiny_typedef.h>
#include <TinyList.h>
#include <selector/Selector.h>
#include <tiny_socket.h>
#include "../../ByteBuffer/ByteBuffer.h"
#include "ChannelAddress.h"
#include "ChannelTimer.h"

TINY_BEGIN_DECLS


#define CHANNEL_ID_LEN              64

struct _Channel;
typedef struct _Channel Channel;

typedef void (* ChannelHandleRegister)(Channel *channel, Selector *selector, ChannelTimer *timer);
typedef void (* ChannelHandleRemove)(Channel *channel);
typedef void (* ChannelHandleActive)(Channel *thiz);
typedef void (* ChannelHandleInactive)(Channel *thiz);
typedef void (* ChannelHandleEventTriggered)(Channel *thiz, ChannelTimer *timer);
typedef void (* ChannelHandleClose)(Channel *thiz);
typedef void (* ChannelLoopHook)(Channel *thiz, void *ctx);
typedef TinyRet (* ChannelTimeoutGetter)(Channel *thiz, ChannelTimer *timer, void *ctx);
typedef TinyRet (* ChannelHandleAccess)(Channel* channel, Selector* selector);

struct _Channel
{
    char                            id[CHANNEL_ID_LEN];
    int                             fd;
    ByteBuffer                    * recvBuffer;
    TinyList                        sendBuffers;
    uint32_t                        inBufferSize;
//    uint32_t                        outBufferSize;
    ChannelType                     type;
    ChannelAddress                  local;
    ChannelAddress                  remote;
    ChannelHandleRegister           _onRegister;
    ChannelHandleRemove             _onRemove;
    ChannelHandleActive             _onActive;
    ChannelHandleInactive           _onInactive;
    ChannelHandleAccess             _onAccess;
    ChannelHandleEventTriggered     _onEventTriggered;
    ChannelTimeoutGetter            _getTimeout;
    ChannelHandleClose              _close;
    ChannelLoopHook                 _loopHook;
    void                          * _loopHookContext;
    TinyList                        handlers;
    int                             currentReader;
    int                             currentWriter;
    void                          * context;
};

#define Channel_IsActive(thiz)      ((thiz)->fd >= 0)
#define Channel_IsClosed(thiz)      ((thiz)->fd < 0)

TINY_API
TINY_LOR
void Channel_Close(Channel *thiz);

TINY_API
TINY_LOR
void Channel_PreLoop(Channel *thiz);


TINY_END_DECLS

#endif /* __CHANNEL_H__ */
