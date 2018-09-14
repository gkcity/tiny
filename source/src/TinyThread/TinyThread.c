/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyThread.c
 *
 * @remark
 *
 */

#include "TinyThread.h"
#include "../TinyPortable/tiny_malloc.h"
#include "tiny_log.h"

#define TAG                 "TinyThread"
#define THREAD_UNNAMED      "unnamed"

static void * thread_run(void *param);

TinyThread * TinyThread_New(void)
{
    TinyThread *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TinyThread *)tiny_malloc(sizeof(TinyThread));
        if (thiz == NULL)
        {
            break;
        }

        ret = TinyThread_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TinyThread_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet TinyThread_Construct(TinyThread *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(TinyThread));

    return TINY_RET_OK;
}

TinyRet TinyThread_Initialize(TinyThread *thiz, ThreadLoop loop, void *param, const char *name)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    if (name != NULL)
    {
        strncpy(thiz->name, name, THREAD_NAME_LEN);
    }
    else
    {
        strncpy(thiz->name, THREAD_UNNAMED, THREAD_NAME_LEN);
    }

    thiz->loop = loop;
    thiz->thread_param = param;
    thiz->status = ThreadInit;

#ifdef _WIN32
    thiz->thread_handler = NULL;
    thiz->thread_id = 0;
#else /* Linux */
    thiz->thread_handler = 0;
    thiz->thread_id = 0;
#endif /* _WIN32 */

    return TINY_RET_OK;
}

TinyRet TinyThread_Dispose(TinyThread *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TinyThread_Join(thiz);

    memset(thiz->name, 0, THREAD_NAME_LEN);

    thiz->loop = NULL;
    thiz->thread_param = NULL;
    thiz->status = ThreadInit;

#ifdef _WIN32
    thiz->thread_handler = NULL;
    thiz->thread_id = 0;
#else /* Linux */
    thiz->thread_handler = 0;
    thiz->thread_id = 0;
#endif /* _WIN32 */

    return TINY_RET_OK;
}

void TinyThread_Delete(TinyThread *thiz)
{
    TinyThread_Dispose(thiz);
    tiny_free(thiz);
}

const char * TinyThread_GetName(TinyThread *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->name;
}

bool TinyThread_Start(TinyThread *thiz)
{
#ifdef _WIN32
#else
    int ret = 0;
#endif

    RETURN_VAL_IF_FAIL(thiz, false);

    LOG_D(TAG, "TinyThread_Start: %s", thiz->name);

    if (thiz->status == ThreadRunning)
    {
        return false;
    }

    if (thiz->loop == NULL)
    {
        return false;
    }

#ifdef _WIN32
    thiz->thread_handler = CreateThread(NULL,
        0,
        (LPTHREAD_START_ROUTINE)thread_run,
        (LPVOID)thiz,
        0,
        &thiz->thread_id);

#else
    typedef void* (*LPTHREAD_START_ROUTINE)(void *);
    ret = pthread_create(&thiz->thread_id,
        NULL,
        (LPTHREAD_START_ROUTINE)thread_run,
        (void*)thiz);

    (void)ret;
    thiz->thread_handler = (int)thiz->thread_id;
#endif

    return true;
}

bool TinyThread_Join(TinyThread *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    if (thiz->status == ThreadRunning)
    {
        LOG_D(TAG, "TinyThread_Join: %s", thiz->name);

#ifdef _WIN32
        if (thiz->thread_handler != INVALID_HANDLE_VALUE)
        {
            WaitForSingleObject(thiz->thread_handler, INFINITE);
            CloseHandle(thiz->thread_handler);
            thiz->thread_handler = INVALID_HANDLE_VALUE;
        }
#else
        void *status = NULL;

        if (thiz->thread_handler != 0)
        {
            pthread_join(thiz->thread_id, &status);
            thiz->thread_handler = 0;
        }
#endif

        LOG_D(TAG, "finished: %s", thiz->name);
    }

    return true;
}

static void * thread_run(void *param)
{
    TinyThread *thiz = (TinyThread *)param;
    thiz->status = ThreadRunning;
    thiz->loop(thiz->thread_param);
    thiz->status = ThreadStop;

    return NULL;
}
