/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyBlockingQueue.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#ifndef __TINY_BLOCKING_QUEUE_H__
#define __TINY_BLOCKING_QUEUE_H__

#include "tiny_base.h"
#include "TinyMutex.h"
#include "TinySemaphore.h"
#include "TinyQueue.h"

TINY_BEGIN_DECLS


typedef struct _TinyBlockingQueue
{
    TinyQueue                 queue;
    TinyMutex                 mutex;
    TinySemaphore             sem;
} TinyBlockingQueue;

TINY_API TinyBlockingQueue * TinyBlockingQueue_New(void);
TINY_API TinyRet TinyBlockingQueue_Construct(TinyBlockingQueue *thiz);
TINY_API TinyRet TinyBlockingQueue_Dispose(TinyBlockingQueue *thiz);
TINY_API void TinyBlockingQueue_Delete(TinyBlockingQueue *thiz);

/* put a job */
TINY_API void TinyBlockingQueue_Put(TinyBlockingQueue *thiz, void *job);

/* take a job (block) */
TINY_API void * TinyBlockingQueue_Take(TinyBlockingQueue *thiz);

/* get & remove a job (nonblock) */
TINY_API void * TinyBlockingQueue_Poll(TinyBlockingQueue *thiz);

TINY_API void TinyBlockingQueue_Stop(TinyBlockingQueue *thiz);


TINY_END_DECLS

#endif /* __TINY_BLOCKING_QUEUE_H__ */
