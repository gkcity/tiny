/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyWorker.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include "TinyWorker.h"
#include "../TinyPortable/tiny_malloc.h"
#include "tiny_log.h"

#define TAG     "TinyWorker"
static void worker_loop(void *param);

TinyWorker * TinyWorker_New(void)
{
    TinyWorker *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TinyWorker *)tiny_malloc(sizeof(TinyWorker));
        if (thiz == NULL)
        {
            break;
        }

        ret = TinyWorker_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TinyWorker_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet TinyWorker_Construct(TinyWorker *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(TinyWorker));

        thiz->is_running = false;

        ret = TinyBlockingQueue_Construct(&thiz->job_queue);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyBlockingQueue_Construct failed");
            break;
        }

        ret = TinyThread_Construct(&thiz->thread);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyThread_Construct failed");
            break;
        }
    }
    while (0);

    return ret;
}

TinyRet TinyWorker_Initialize(TinyWorker *thiz, TinyWorkerJobDeleteListener listener, void *ctx)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);

    thiz->job_delete_listener = listener;
    thiz->job_delete_listener_ctx = ctx;

    return TINY_RET_OK;
}

TinyRet TinyWorker_Dispose(TinyWorker *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    if (thiz->is_running)
    {
        TinyWorker_Stop(thiz);
    }

    while (true)
    {
        void *job = TinyBlockingQueue_Poll(&thiz->job_queue);
        if (job == NULL)
        {
            break;
        }

        if (thiz->job_delete_listener != NULL)
        {
            thiz->job_delete_listener(thiz, job, thiz->job_delete_listener_ctx);
        }
        else
        {
            LOG_E(TAG, "job is not delete!");
        }
    }

    TinyBlockingQueue_Dispose(&thiz->job_queue);
    TinyThread_Dispose(&thiz->thread);

    return TINY_RET_OK;
}

void TinyWorker_Delete(TinyWorker *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyWorker_Dispose(thiz);
    tiny_free(thiz);
}

const char * TinyWorker_GetName(TinyWorker *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return TinyThread_GetName(&thiz->thread);
}

TinyRet TinyWorker_Start(TinyWorker *thiz, const char *name,  TinyWorkerListener listener, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(listener, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->is_running)
        {
            ret = TINY_RET_E_STARTED;
            break;
        }

        ret = TinyThread_Initialize(&thiz->thread, worker_loop, thiz, name);
        if (RET_FAILED(ret))
        {
            break;
        }

        thiz->is_running = true;
        thiz->listener = listener;
        thiz->listener_ctx = ctx;

        TinyThread_Start(&thiz->thread);

        ret = TINY_RET_OK;
    }
    while (0);

    return ret;
}

TinyRet TinyWorker_Stop(TinyWorker *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    if (thiz->is_running)
    {
        thiz->is_running = false;

        thiz->listener = NULL;
        thiz->listener_ctx = NULL;

        TinyBlockingQueue_Stop(&thiz->job_queue);
        TinyThread_Join(&thiz->thread);
        return TINY_RET_OK;
    }

    return TINY_RET_E_STOPPED;
}

bool TinyWorker_IsStarted(TinyWorker *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);
    return thiz->is_running;
}

TinyRet TinyWorker_PutJob(TinyWorker *thiz, void *job)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(job, TINY_RET_E_ARG_NULL);

    TinyBlockingQueue_Put(&thiz->job_queue, job);
    return TINY_RET_OK;
}

void * TinyWorker_GetJob(TinyWorker *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return TinyBlockingQueue_Poll(&thiz->job_queue);
}

static void worker_loop(void *param)
{
    TinyWorker *thiz = (TinyWorker *)param;

    while (1)
    {
        void *job = TinyBlockingQueue_Take(&thiz->job_queue);
        if (job == NULL)
        {
            break;
        }

        if (! thiz->is_running)
        {
            LOG_D(TAG, "worker is stopped");

            if (thiz->job_delete_listener != NULL)
            {
                thiz->job_delete_listener(thiz, job, thiz->job_delete_listener_ctx);
            }
            else
            {
                LOG_E(TAG, "job is not delete!");
            }

            break;
        }

        if (thiz->listener == NULL)
        {
            LOG_D(TAG, "worker listener is NULL");
            break;
        }

        if (!thiz->listener(thiz, job, thiz->listener_ctx))
        {
            break;
        }
    }
}
