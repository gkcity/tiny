/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   UnicastChannelContext.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __UNICAST_CHANNEL_CONTEXT_H__
#define __UNICAST_CHANNEL_CONTEXT_H__

#include <tiny_typedef.h>
#include <tiny_ret.h>
#include <TinyList.h>
#include "channel/ChannelInitializer.h"

TINY_BEGIN_DECLS


typedef struct _UnicastChannelContext
{
    uint16_t                      port;
} UnicastChannelContext;

TINY_API
TINY_LOR
UnicastChannelContext * UnicastChannelContext_New(void);

TINY_API
TINY_LOR
void UnicastChannelContext_Delete(UnicastChannelContext *thiz);


TINY_END_DECLS

#endif /* __UNICAST_CHANNEL_CONTEXT_H__ */