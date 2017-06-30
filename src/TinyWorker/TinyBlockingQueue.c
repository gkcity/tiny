/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyBlockingQueue.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include "TinyBlockingQueue.h"
#include "../TinyPortable/tiny_malloc.h"
#include "tiny_log.h"

#define TAG     "TinyBlockingQueue"

TinyBlockingQueue * TinyBlockingQueue_New(void)
{
    TinyBlockingQueue *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TinyBlockingQueue *)tiny_malloc(sizeof(TinyBlockingQueue));
        if (thiz == NULL)
        {
            break;
        }

        ret = TinyBlockingQueue_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TinyBlockingQueue_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet TinyBlockingQueue_Construct(TinyBlockingQueue *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(TinyBlockingQueue));

    do
    {
        ret = TinyQueue_Construct(&thiz->queue);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyQueue_Construct failed");
            break;
        }

        ret = TinyMutex_Construct(&thiz->mutex);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyMutex_Construct failed");
            break;
        }

        ret = TinySemaphore_Construct(&thiz->sem);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinySemaphore_Construct failed");
            break;
        }
    }
    while (0);

    return ret;
}

TinyRet TinyBlockingQueue_Dispose(TinyBlockingQueue *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    
    TinySemaphore_Dispose(&thiz->sem);
    TinyMutex_Dispose(&thiz->mutex);
    TinyQueue_Dispose(&thiz->queue);

    return TINY_RET_OK;
}

void TinyBlockingQueue_Delete(TinyBlockingQueue *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyBlockingQueue_Dispose(thiz);

    tiny_free(thiz);
}

void TinyBlockingQueue_Put(TinyBlockingQueue *thiz, void *job)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(job);

    TinyMutex_Lock(&thiz->mutex);
    {
        TinyQueue_Push(&thiz->queue, job);
    }
    TinyMutex_Unlock(&thiz->mutex);

    TinySemaphore_Post(&thiz->sem);
}

void * TinyBlockingQueue_Take(TinyBlockingQueue *thiz)
{
    void *data = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);

    //LOG_D(TAG, "TinyBlockingQueue_Take ...");

    TinySemaphore_Wait(&thiz->sem);

    //LOG_D(TAG, "before take, q size is: %u", (uint32_t)TinyQueue_GetSize(&thiz->q));

    TinyMutex_Lock(&thiz->mutex);
    {
        data = TinyQueue_Head(&thiz->queue);
        TinyQueue_Pop(&thiz->queue);
    }
    TinyMutex_Unlock(&thiz->mutex);

    //LOG_D(TAG, "TinyBlockingQueue_Take ok");

    return data;
}

void * TinyBlockingQueue_Poll(TinyBlockingQueue *thiz)
{
    void *data = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);

    TinyMutex_Lock(&thiz->mutex);
    {
        data = TinyQueue_Head(&thiz->queue);
        TinyQueue_Pop(&thiz->queue);
    }
    TinyMutex_Unlock(&thiz->mutex);

    return data;
}

void TinyBlockingQueue_Stop(TinyBlockingQueue *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinySemaphore_Post(&thiz->sem);
}