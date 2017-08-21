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
#include "ChannelAddress.h"

TINY_BEGIN_DECLS


#define CHANNEL_ID_LEN              64

struct _Channel;
typedef struct _Channel Channel;

typedef void (* ChannelHandleRegister)(Channel *channel, Selector *selector);
typedef void (* ChannelHandleRemove)(Channel *channel);
typedef void (* ChannelHandleActive)(Channel *thiz);
typedef void (* ChannelHandleInactive)(Channel *thiz);
typedef void (* ChannelHandleEventTriggered)(Channel *thiz, void *event);
typedef TinyRet (* ChannelHandleAccess)(Channel* channel, Selector* selector);
typedef int64_t (* ChannelTimeoutGetter)(Channel *thiz, void *ctx);

struct _Channel
{
    char                            id[CHANNEL_ID_LEN];
    int                             fd;
    ChannelType                     type;
    ChannelAddress                  local;
    ChannelAddress                  remote;
    ChannelHandleRegister           onRegister;
    ChannelHandleRemove             onRemove;
    ChannelHandleActive             onActive;
    ChannelHandleInactive           onInactive;
    ChannelHandleAccess             onReadWrite;
    ChannelHandleEventTriggered     onEventTriggered;
    ChannelTimeoutGetter            getNextTimeout;
    TinyList                        handlers;
    int                             currentReader;
    int                             currentWriter;
    void                          * ctx;
};

#define Channel_IsActive(thiz)      ((thiz)->fd >= 0)
#define Channel_IsClosed(thiz)      ((thiz)->fd < 0)
#define Channel_Close(thiz)         {tiny_socket_close((thiz)->fd); (thiz)->fd = -1;}


TINY_END_DECLS

#endif /* __CHANNEL_H__ */
