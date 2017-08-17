/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyCondition.c
 *
 * @remark
 *
 */

#include "TinyCondition.h"
#include "../TinyPortable/tiny_malloc.h"
#include "tiny_log.h"

#define TAG     "TinyCondition"

TinyCondition * TinyCondition_New(void)
{
    TinyCondition *thiz = NULL;
    
    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TinyCondition *)tiny_malloc(sizeof(TinyCondition));
        if (thiz == NULL)
        {
            break;
        }

        ret = TinyCondition_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TinyCondition_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet TinyCondition_Construct(TinyCondition *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(TinyCondition));

        thiz->is_ready = false;

        ret = TinyMutex_Construct(&thiz->mutex);
        if (RET_FAILED(ret))
        {
            break;
        }

#ifdef _WIN32
        thiz->job = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (thiz->job == NULL)
        {
            LOG_E(TAG, "TinyCondition_Construct: CreateEvent failed");
            ret = TINY_RET_E_INTERNAL;
            break;
        }
#else
        if (pthread_cond_init(&thiz->job, NULL) != 0)
        {
            LOG_E(TAG, "TinyCondition_Construct: pthread_cond_init failed");
            ret = TINY_RET_E_INTERNAL;
            break;
        }
#endif

        ret = TINY_RET_OK;
    }
    while (false);

    return ret;
}

TinyRet TinyCondition_Dispose(TinyCondition *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TinyMutex_Dispose(&thiz->mutex);

#ifdef _WIN32
    if (thiz->job)
    {
        CloseHandle(thiz->job);
    }
#else
    pthread_cond_destroy(&thiz->job);
#endif

    return TINY_RET_OK;
}

void TinyCondition_Delete(TinyCondition *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyCondition_NotifyAll(thiz);
    TinyCondition_Dispose(thiz);
    tiny_free(thiz);
}

bool TinyCondition_Wait(TinyCondition *thiz)
{
    bool result = false;

    RETURN_VAL_IF_FAIL(thiz, false);

#ifdef _WIN32
    switch (WaitForSingleObject(thiz->job, INFINITE))
    {
    case WAIT_OBJECT_0:
        result = true;
        break;

        /* ָ���Ķ�����һ��������󣬸ö���û�б�ӵ�иö�����߳����߳̽���ǰ�ͷš�
        ������������Ȩ��ͬ��������øú������̡߳�����������ó�Ϊ���ź�״̬��*/
    case WAIT_ABANDONED:
        result = false;
        break;

    case WAIT_TIMEOUT:
        result = false;
        break;

    default:
        result = false;
        break;
    }
#else
    result = true;

    TinyMutex_Lock(&thiz->mutex);
    thiz->is_ready = false;

    while (thiz->is_ready == false)
    {
        pthread_cond_wait(&thiz->job, &(thiz->mutex.mutex));
    }

    TinyMutex_Unlock(&thiz->mutex);
#endif

    return result;
}

bool TinyCondition_NotifyOne(TinyCondition *thiz)
{
    bool ret = true;

    RETURN_VAL_IF_FAIL(thiz, false);

#ifdef _WIN32
    SetEvent(thiz->job);
#else

    TinyMutex_Lock(&thiz->mutex);

    do
    {
        thiz->is_ready = true;

        if (pthread_cond_signal(&thiz->job) != 0)
        {
            ret = false;
            break;
        }

        ret = true;
    }
    while (0);

    TinyMutex_Unlock(&thiz->mutex);
#endif

    return ret;
}

bool TinyCondition_NotifyAll(TinyCondition *thiz)
{
    bool ret = true;

    RETURN_VAL_IF_FAIL(thiz, false);

#ifdef _WIN32
    SetEvent(thiz->job);
#else

    TinyMutex_Lock(&thiz->mutex);

    do
    {
        thiz->is_ready = true;

        if (pthread_cond_broadcast(&thiz->job) != 0)
        {
            ret = false;
            break;
        }

        ret = true;
    }
    while (0);

    TinyMutex_Unlock(&thiz->mutex);
#endif

    return ret;
}
