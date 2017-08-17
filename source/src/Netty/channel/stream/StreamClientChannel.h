/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   StreamServerChannel.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __STREAM_CLIENT_CHANNEL_H__
#define __STREAM_CLIENT_CHANNEL_H__

#include <tiny_typedef.h>
#include <tiny_ret.h>
#include "channel/Channel.h"
#include "channel/ChannelInitializer.h"

TINY_BEGIN_DECLS


Channel * StreamClientChannel_New();

TinyRet StreamClientChannel_Initialize(Channel *thiz, ChannelInitializer initializer, void *ctx);
TinyRet StreamClientChannel_Connect(Channel *thiz, const char *ip, uint16_t port);
TinyRet StreamClientChannel_Close(Channel *thiz);
void StreamClientChannel_Delete(Channel *thiz);
bool StreamClientChannel_isConnected(Channel* thiz);



TINY_END_DECLS

#endif /* __STREAM_CLIENT_CHANNEL_H__ */

