/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   UnicastChannel.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __UNICAST_CHANNEL_H__
#define __UNICAST_CHANNEL_H__

#include <tiny_typedef.h>
#include <tiny_ret.h>
#include "channel/Channel.h"
#include "channel/ChannelInitializer.h"


TINY_BEGIN_DECLS


TINY_API
TINY_LOR
Channel * UnicastChannel_New(void);

TINY_API
TINY_LOR
void UnicastChannel_Initialize(Channel *thiz, ChannelInitializer initializer, void *ctx);

TINY_API
TINY_LOR
TinyRet UnicastChannel_Open(Channel *thiz, const char *ip, uint16_t port, bool reuse);

TINY_API
TINY_LOR
TinyRet UnicastChannel_WriteTo(Channel *thiz, const void *data, uint32_t len, uint32_t ip, uint16_t port);


TINY_END_DECLS

#endif /* __UNICAST_CHANNEL_H__ */