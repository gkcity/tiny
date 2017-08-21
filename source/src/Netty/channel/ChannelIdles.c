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
#include <tiny_base.h>
#include "ChannelIdles.h"

#define TAG         "ChannelIdle"

TINY_LOR
static void ChannelIdle_Initialize(ChannelIdle *thiz, ChannelTimerType type, uint32_t idle, uint64_t current)
{
    thiz->type = type;

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
static bool ChannelIdle_GetNextTimeout(ChannelIdle *thiz, uint64_t current, ChannelTimer *timer)
{
    int64_t timeout = 0;

//    LOG_D(TAG, "[%d] timer->timeout: %ds %dms %dus", timer->fd,
//          (int) (timer->timeout / 1000000), (int) (timer->timeout % 1000000) / 1000, (int) (timer->timeout % 1000));
//
//    LOG_D(TAG, "interval: %ds", thiz->interval / 1000000);
//
//    LOG_D(TAG, "triggerPoint: %ds %dms %dus",
//          (int) (thiz->triggerPoint / 1000000), (int) (thiz->triggerPoint % 1000000) / 1000, (int) (thiz->triggerPoint % 1000));

    if (thiz->interval == 0)
    {
        return false;
    }

    if (current > thiz->triggerPoint)
    {
        thiz->triggerPoint = current + 1000;
//        LOG_D(TAG, "correct triggerPoint: %ds %dms %dus",
//              (int) (thiz->triggerPoint / 1000000), (int) (thiz->triggerPoint % 1000000) / 1000, (int) (thiz->triggerPoint % 1000));
    }

    timeout = (int64_t) (thiz->triggerPoint - current);

//    LOG_D(TAG, "timeout: %d.%d.%d", (int) (timeout / 1000000), (int) (timeout % 1000000) / 1000, (int) (timeout % 1000));

    if ((timer->valid && (timeout < timer->timeout)) || (!timer->valid))
    {
        timer->valid = true;
        timer->type = thiz->type;
        timer->timeout = timeout;
    }

    return true;
}

TINY_LOR
void ChannelIdles_Initialize(ChannelIdles *thiz, uint32_t readerIdle, uint32_t writerIdle, uint32_t allIdle)
{
    uint64_t current = tiny_current_microsecond();
    ChannelIdle_Initialize(&thiz->reader, CHANNEL_TIMER_READER, readerIdle, current);
    ChannelIdle_Initialize(&thiz->writer, CHANNEL_TIMER_WRITER, writerIdle, current);
    ChannelIdle_Initialize(&thiz->all, CHANNEL_TIMER_ALL, allIdle, current);
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
void ChannelIdles_OnAll(ChannelIdles *thiz)
{
    uint64_t current = tiny_current_microsecond();
    ChannelIdle_Update(&thiz->all, current);
}

TINY_LOR
TinyRet ChannelIdles_GetTimeout(ChannelIdles *thiz, void *ctx)
{
    TinyRet ret = TINY_RET_E_NOT_FOUND;
    ChannelTimer *timer = (ChannelTimer *)ctx;
    uint64_t current = tiny_current_microsecond();

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(timer, TINY_RET_E_ARG_NULL);

    printf("\n---------------------------------------------------------\n");

//    LOG_D(TAG, "current: %d.%d.%d", (int) (current / 1000000), (int) (current % 1000000) / 1000, (int) (current % 1000));

    if (ChannelIdle_GetNextTimeout(&thiz->reader, current, timer))
    {
//        LOG_D(TAG, "reader: %d.%d.%d", (int) (timer->timeout / 1000000), (int) (timer->timeout % 1000000) / 1000, (int) (timer->timeout % 1000));
        ret = TINY_RET_OK;
    }

    if (ChannelIdle_GetNextTimeout(&thiz->writer, current, timer))
    {
//        LOG_D(TAG, "writer: %d.%d.%d", (int) (timer->timeout / 1000000), (int) (timer->timeout % 1000000) / 1000, (int) (timer->timeout % 1000));
        ret = TINY_RET_OK;
    }

    if (ChannelIdle_GetNextTimeout(&thiz->all, current, timer))
    {
//        LOG_D(TAG, "all: %d.%d.%d", (int) (timer->timeout / 1000000), (int) (timer->timeout % 1000000) / 1000, (int) (timer->timeout % 1000));
        ret = TINY_RET_OK;
    }

    return ret;
}

TINY_LOR
void ChannelIdles_OnEvent(ChannelIdles *thiz, void *event)
{
    ChannelTimer *timer = (ChannelTimer *)event;
    uint64_t current = tiny_current_microsecond();

    switch (timer->type)
    {
        case CHANNEL_TIMER_READER:
            ChannelIdle_Update(&thiz->reader, current);
            break;

        case CHANNEL_TIMER_WRITER:
            ChannelIdle_Update(&thiz->writer, current);
            break;

        case CHANNEL_TIMER_ALL:
            ChannelIdle_Update(&thiz->all, current);
            break;
    }
}