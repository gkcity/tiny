/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyWorker.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#ifndef __TINY_WORKER_H__
#define __TINY_WORKER_H__

#include "tiny_base.h"
#include "TinyBlockingQueue.h"
#include "TinyThread.h"

TINY_BEGIN_DECLS


struct _TinyWorker;
typedef struct _TinyWorker TinyWorker;

typedef bool(*TinyWorkerListener)(TinyWorker *worker, void *job, void *ctx);
typedef void(*TinyWorkerJobDeleteListener)(TinyWorker *worker, void *job, void *ctx);

struct _TinyWorker
{
    bool                            is_running;
    TinyBlockingQueue               job_queue;
    TinyWorkerListener              listener;
    void                          * listener_ctx;
    TinyThread                      thread;
    TinyWorkerJobDeleteListener     job_delete_listener;
    void                          * job_delete_listener_ctx;
};

TINY_API TinyWorker * TinyWorker_New(void);
TINY_API TinyRet TinyWorker_Construct(TinyWorker *thiz);
TINY_API TinyRet TinyWorker_Initialize(TinyWorker *thiz, TinyWorkerJobDeleteListener listener, void *ctx);
TINY_API TinyRet TinyWorker_Dispose(TinyWorker *thiz);
TINY_API void TinyWorker_Delete(TinyWorker *thiz);

TINY_API const char * TinyWorker_GetName(TinyWorker *thiz);
TINY_API TinyRet TinyWorker_Start(TinyWorker *thiz, const char *name, TinyWorkerListener listener, void *ctx);
TINY_API TinyRet TinyWorker_Stop(TinyWorker *thiz);
TINY_API bool TinyWorker_IsStarted(TinyWorker *thiz);

TINY_API TinyRet TinyWorker_PutJob(TinyWorker *thiz, void *job);
TINY_API void * TinyWorker_GetJob(TinyWorker *thiz);


TINY_END_DECLS

#endif /* __TINY_WORKER_H__ */
