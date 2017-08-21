/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ChannelIdle.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_time.h>
#include <tiny_log.h>
#include "ChannelIdles.h"

#define TAG         "ChannelIdle"


TINY_LOR
static void ChannelIdle_Initialize(ChannelIdle *thiz, uint32_t idle, uint64_t current)
{
    if (idle > 0)
    {
        thiz->interval = idle * 1000000;
        thiz->triggerPoint = current + thiz->interval;
    }
}

TINY_LOR
static void ChannelIdle_Update(ChannelIdle *thiz, uint64_t current)
{
    if (thiz->interval > 0)
    {
        thiz->triggerPoint = current + thiz->interval;
    }
}

TINY_LOR
static int64_t ChannelIdle_GetNextTimeout(ChannelIdle *thiz, uint64_t current, int64_t defaultTimeout)
{
    int64_t timeout = 1000;

    if (thiz->interval == 0)
    {
        return defaultTimeout;
    }

    if (current > thiz->triggerPoint)
    {
        thiz->triggerPoint = current + timeout;
    }
    else
    {
        timeout = (int64_t) (thiz->triggerPoint - current);
    }

    return (timeout < defaultTimeout) ? timeout : defaultTimeout;
}

TINY_LOR
void ChannelIdles_Initialize(ChannelIdles *thiz, uint32_t readerIdle, uint32_t writerIdle, uint32_t allIdle)
{
    uint64_t current = tiny_current_microsecond();
    ChannelIdle_Initialize(&thiz->reader, readerIdle, current);
    ChannelIdle_Initialize(&thiz->writer, writerIdle, current);
    ChannelIdle_Initialize(&thiz->all, allIdle, current);
}

TINY_LOR
void ChannelIdles_OnRead(ChannelIdles *thiz)
{
    uint64_t current = tiny_current_microsecond();
    ChannelIdle_Update(&thiz->reader, current);
    ChannelIdle_Update(&thiz->all, current);
}

TINY_LOR
void ChannelIdles_OnWrite(ChannelIdles *thiz)
{
    uint64_t current = tiny_current_microsecond();
    ChannelIdle_Update(&thiz->writer, current);
    ChannelIdle_Update(&thiz->all, current);
}

TINY_LOR
int64_t ChannelIdles_GetNextTimeout(ChannelIdles *thiz)
{
    uint64_t current = tiny_current_microsecond();
    int64_t timeout = 0;

    timeout = ChannelIdle_GetNextTimeout(&thiz->reader, current, timeout);
    timeout = ChannelIdle_GetNextTimeout(&thiz->writer, current, timeout);
    timeout = ChannelIdle_GetNextTimeout(&thiz->all, current, timeout);

    LOG_D(TAG, "ChannelIdles_GetNextTimeout: %ld us", timeout);

    return timeout;
}