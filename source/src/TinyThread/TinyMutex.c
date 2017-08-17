/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyMutex.c
 *
 * @remark
 *
 */

#include "TinyMutex.h"
#include "../TinyPortable/tiny_malloc.h"
#include "tiny_log.h"

#define TAG     "TinyMutex"

TinyMutex * TinyMutex_New(void)
{
    TinyMutex *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TinyMutex *)tiny_malloc(sizeof(TinyMutex));
        if (thiz == NULL)
        {
            break;
        }

        ret = TinyMutex_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TinyMutex_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet TinyMutex_Construct(TinyMutex *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(TinyMutex));

#ifdef _WIN32
    InitializeCriticalSection(&thiz->mutex);
#else
    if (pthread_mutex_init(&thiz->mutex, NULL) != 0)
    {
        LOG_E(TAG, "TinyMutex_Construct: pthread_mutex_init failed");
        return TINY_RET_E_INTERNAL;
    }
#endif

    return TINY_RET_OK;
}

TinyRet TinyMutex_Dispose(TinyMutex *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

#ifdef _WIN32
    DeleteCriticalSection(&thiz->mutex);
#else
    pthread_mutex_destroy(&thiz->mutex);
#endif

    return TINY_RET_OK;
}

void TinyMutex_Delete(TinyMutex *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyMutex_Dispose(thiz);
    tiny_free(thiz);
}

bool TinyMutex_Lock(TinyMutex *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

#ifdef _WIN32
    EnterCriticalSection(&thiz->mutex);
#else
    if (pthread_mutex_lock(&thiz->mutex) != 0)
    {
        LOG_W(TAG, "TinyMutex_Lock: pthread_mutex_lock failed");
        return false;
    }
#endif

    return true;
}

bool TinyMutex_Unlock(TinyMutex *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

#ifdef _WIN32
    LeaveCriticalSection(&thiz->mutex);
#else
    if (pthread_mutex_unlock(&thiz->mutex) != 0)
    {
        LOG_W(TAG, "TinyMutex_Lock: pthread_mutex_unlock failed");
        return false;
    }
#endif

    return true;
}