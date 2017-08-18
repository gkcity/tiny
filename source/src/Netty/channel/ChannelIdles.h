/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ChannelIdle.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __CHANNEL_IDLE_H__
#define __CHANNEL_IDLE_H__

#include <tiny_typedef.h>
#include <tiny_ret.h>

TINY_BEGIN_DECLS


typedef struct _ChannelIdle
{
    uint32_t        interval;
    uint64_t        triggerPoint;
} ChannelIdle;

typedef struct _ChannelIdles
{
    ChannelIdle     reader;
    ChannelIdle     writer;
    ChannelIdle     all;
} ChannelIdles;

TINY_LOR
void ChannelIdles_Initialize(ChannelIdles *thiz, uint32_t readerIdle, uint32_t writerIdle, uint32_t allIdle);

TINY_LOR
void ChannelIdles_OnRead(ChannelIdles *thiz);

TINY_LOR
void ChannelIdles_OnWrite(ChannelIdles *thiz);

TINY_LOR
int64_t ChannelIdles_GetNextTimeout(ChannelIdles *thiz);


TINY_END_DECLS

#endif /* __CHANNEL_IDLE_H__ */
