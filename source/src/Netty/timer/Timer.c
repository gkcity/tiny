/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   Timer.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_ret.h>
#include <tiny_log.h>
#include <tiny_malloc.h>
#include "Timer.h"

#define TAG     "Timer"

static TinyRet Timer_Construct(Timer *thiz, const char *id, int64_t elapsed, void *ctx, TimerHandler handler)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    LOG_D(TAG, "Timer_Construct");
    memset(thiz, 0, sizeof(Timer));
    strncpy(thiz->id, id, TIMER_ID_LEN);
    thiz->elapsedUS = elapsed;
    thiz->ctx = ctx;
    thiz->handler = handler;

    return ret;
}

Timer * Timer_New(const char *id, int64_t elapsed, void *ctx, TimerHandler handler)
{
    Timer *thiz = NULL;

    do
    {
        thiz = (Timer *)tiny_malloc(sizeof(Timer));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(Timer_Construct(thiz, id, elapsed, ctx, handler)))
        {
            Timer_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet Timer_Dispose(Timer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    LOG_D(TAG, "Timer_Dispose");

    return TINY_RET_OK;
}

void Timer_Delete(Timer *thiz)
{
    Timer_Dispose(thiz);
    tiny_free(thiz);
}