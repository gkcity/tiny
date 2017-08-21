/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ChannelTimer.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __CHANNEL_TIMER_H__
#define __CHANNEL_TIMER_H__

#include <tiny_typedef.h>
#include <tiny_ret.h>
#include "ChannelIdles.h"

TINY_BEGIN_DECLS


typedef struct _ChannelTimer
{
    bool                valid;
    int                 fd;
    ChannelIdleType     idleType;
    int64_t             timeout;
} ChannelTimer;


TINY_END_DECLS

#endif /* __CHANNEL_TIMER_H__ */
