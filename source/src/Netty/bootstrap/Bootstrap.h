/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   Bootstrap.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __BOOTSTRAP_H__
#define __BOOTSTRAP_H__

#include <tiny_typedef.h>
#include <tiny_ret.h>
#include <channel/Channel.h>
#include <channel/ChannelTimer.h>
#include <selector/Selector.h>

TINY_BEGIN_DECLS


#define EVENT_LOOP_DEFAULT_TIMEOUT  (1000 * 1000 * 3)


struct _Bootstrap;
typedef struct _Bootstrap Bootstrap;

typedef void (* BootstrapLoopHook)(Bootstrap *thiz, void *ctx);

struct _Bootstrap
{
    Selector                    selector;
    TinyList                    channels;
    ChannelTimer                timer;
    uint32_t                    loopTimeout;
    BootstrapLoopHook           preloop;
    void                      * preloopCtx;
};

TINY_API
TINY_LOR
TinyRet Bootstrap_Construct(Bootstrap *thiz, BootstrapLoopHook preloop, void *ctx);

TINY_API
TINY_LOR
void Bootstrap_Dispose(Bootstrap *thiz);

TINY_API
TINY_LOR
TinyRet Bootstrap_AddChannel(Bootstrap *thiz, Channel *channel);

TINY_API
TINY_LOR
TinyRet Bootstrap_RemoveChannel(Bootstrap *thiz, Channel *channel);

TINY_API
TINY_LOR
TinyRet Bootstrap_Sync(Bootstrap *thiz);

TINY_API
TINY_LOR
void Bootstrap_Shutdown(Bootstrap *thiz);


TINY_END_DECLS

#endif /* __BOOTSTRAP_H__ */
