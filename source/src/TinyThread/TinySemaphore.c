/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinySemaphore.c
 *
 * @remark
 *
 */

#include "TinySemaphore.h"
#include <tiny_malloc.h>
#include <tiny_log.h>
#include <tiny_snprintf.h>
#include <tiny_time.h>


#define TAG     "TinySemaphore"

#ifdef _WIN32
//#else
bool ctx_gen_ipc_name(const char *name, char *full_name, uint32_t len)
{
    const char * dir = NULL;
    const char * slash = NULL;

    dir = getenv("PX_IPC_NAME");
    if (dir == NULL)
    {
    #ifdef POSIX_IPC_PREFIX
        dir = POSIX_IPC_PREFIX;
    #else
        dir = "/tmp/";
    #endif
    }

    /* dir must end in a slash */
    slash = (dir[strlen(dir) - 1] == '/') ? "" : "/";

    if (name == NULL)
    {
        uint64_t usec = tiny_current_microsecond();
    #ifdef __MAC_OSX__
        tiny_snprintf(full_name, len, "%s%s%llu", dir, slash, usec);
    #else
        tiny_snprintf(full_name, len, "%s%s%lu", dir, slash, (unsigned long)usec);
    #endif
    }
    else
    {
        tiny_snprintf(full_name, len, "%s%s%s", dir, slash, name);
    }

    return true;
}
#endif

TinySemaphore * TinySemaphore_New(void)
{
    TinySemaphore *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TinySemaphore *)tiny_malloc(sizeof(TinySemaphore));
        if (thiz == NULL)
        {
            break;
        }

        ret = TinySemaphore_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TinySemaphore_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet TinySemaphore_Construct(TinySemaphore *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(TinySemaphore));

#ifdef _WIN32
        thiz->sem = CreateSemaphore(NULL, 0, MAX_SEM_SIZE, NULL);
        if (thiz->sem == NULL)
        {
            LOG_E(TAG, "TinySemaphore_Initialize: CreateSemaphore failed");
            ret = TINY_RET_E_INTERNAL;
            break;
        }
#endif

#if (defined __LINUX__) || (defined __ANDROID__)
        if (sem_init(&thiz->sem, 0, 0) != 0)
        {
            //LOG_E(TAG, "TinySemaphore_Initialize: sem_init %s", strerror(errno));
            ret = TINY_RET_E_INTERNAL;
            break;
        }
#endif

#ifdef __MAC_OSX__
        ctx_gen_ipc_name(NULL, thiz->sem.name, PATH_MAX);
        thiz->sem.sem = sem_open(thiz->sem.name, O_CREAT, 0600, 0);
        if (thiz->sem.sem == SEM_FAILED)
        {
            LOG_E(TAG, "TinySemaphore_Initialize: sem_open %s", strerror(errno));
            ret = TINY_RET_E_INTERNAL;
            break;
        }
#endif

        ret = TINY_RET_OK;
    }
    while (false);

    return ret;
}

TinyRet TinySemaphore_Dispose(TinySemaphore *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

#ifdef _WIN32
    if (thiz->sem)
    {
        CloseHandle(thiz->sem);
    }
#endif

#if (defined __LINUX__) || (defined __ANDROID__)
    sem_destroy(&thiz->sem);
#endif

#ifdef __MAC_OSX__
    if (thiz->sem.sem)
    {
        sem_close(thiz->sem.sem);
        sem_unlink(thiz->sem.name);
    }
#endif

    return TINY_RET_OK;
}

void TinySemaphore_Delete(TinySemaphore *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinySemaphore_Dispose(thiz);
    tiny_free(thiz);
}

bool TinySemaphore_Wait(TinySemaphore *thiz)
{
    bool result = false;

    RETURN_VAL_IF_FAIL(thiz, false);

#ifdef _WIN32
    switch (WaitForSingleObject(thiz->sem, INFINITE))
    {
    case WAIT_OBJECT_0:
        result = true;
        break;

        /* ָ���Ķ�����һ��������󣬸ö���û�б�ӵ�иö�����߳����߳̽���ǰ�ͷš�
        ������������Ȩ��ͬ��������øú������̡߳�����������ó�Ϊ���ź�״̬��*/
    case WAIT_ABANDONED:
        break;

    case WAIT_TIMEOUT:
        break;

    default:
        break;
    }
#endif

#if (defined __LINUX__) || (defined __ANDROID__)
    if (sem_wait(&thiz->sem) == 0)
    {
        result = true;
    }

#endif

#ifdef __MAC_OSX__
    if (sem_wait(thiz->sem.sem) == 0)
    {
        result = true;
    }
#endif

    return result;
}

bool TinySemaphore_Post(TinySemaphore *thiz)
{
    bool result = false;

    RETURN_VAL_IF_FAIL(thiz, false);

#ifdef _WIN32
    if (ReleaseSemaphore(thiz->sem, 1, NULL) == TRUE)
    {
        result = true;
    }
#endif

#if (defined __LINUX__) || (defined __ANDROID__)
    if (sem_post(&thiz->sem) == 0)
    {
        result = true;
    }
#endif

#ifdef __MAC_OSX__
    if (sem_post(thiz->sem.sem) == 0)
    {
        result = true;
    }
#endif

    return result;
}
