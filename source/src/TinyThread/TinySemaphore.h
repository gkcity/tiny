/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinySemaphore.h
 *
 * @remark
 *
 */

#ifndef __TINY_SEMAPHORE_H__
#define __TINY_SEMAPHORE_H__

#include "tiny_base.h"
#include "tiny_api.h"

TINY_BEGIN_DECLS


#if (defined __LINUX__) || (defined __ANDROID__) || (defined __OPENWRT_MT7688__)
#include <semaphore.h>
#endif

#ifdef __MACOS__
#include <semaphore.h>  
#include <fcntl.h>
#include <limits.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#include <Windows.h>
#define MAX_SEM_SIZE    128
typedef HANDLE          tiny_sem_t;
#endif /* _WIN32 */

#if (defined __LINUX__) || (defined __ANDROID__) || (defined __OPENWRT_MT7688__)
typedef sem_t           tiny_sem_t;
#endif

#ifdef __MACOS__
typedef struct _tiny_sem_t
{
    char                name[PATH_MAX + 1];
    sem_t *             sem;
} tiny_sem_t;
#endif

typedef struct _TinySemaphore
{
    tiny_sem_t           sem;
} TinySemaphore;

TINY_API
TinySemaphore * TinySemaphore_New(void);

TINY_API
TinyRet TinySemaphore_Construct(TinySemaphore *thiz);

TINY_API
TinyRet TinySemaphore_Dispose(TinySemaphore *thiz);

TINY_API
void TinySemaphore_Delete(TinySemaphore *thiz);

TINY_API
bool TinySemaphore_Wait(TinySemaphore *thiz);

TINY_API
bool TinySemaphore_Post(TinySemaphore *thiz);


TINY_END_DECLS

#endif /* __TINY_SEMAPHORE_H__ */
