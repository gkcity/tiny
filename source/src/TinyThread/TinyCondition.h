/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyCondition.h
 *
 * @remark
 *
 */

#ifndef __TINY_CONDITION_H__
#define __TINY_CONDITION_H__

#include "tiny_base.h"
#include "TinyMutex.h"
#include "tiny_api.h"

TINY_BEGIN_DECLS


#ifdef _WIN32
typedef HANDLE              tiny_cond_t;
#else /* Linux */
typedef pthread_cond_t      tiny_cond_t;
#endif /* _WIN32 */

typedef struct _TinyCondition
{
    TinyMutex       mutex;
    tiny_cond_t     job;
    bool            is_ready;
} TinyCondition;

TINY_API TinyCondition * TinyCondition_New(void);
TINY_API TinyRet TinyCondition_Construct(TinyCondition *thiz);
TINY_API TinyRet TinyCondition_Dispose(TinyCondition *thiz);
TINY_API void TinyCondition_Delete(TinyCondition *thiz);

TINY_API bool TinyCondition_Wait(TinyCondition *thiz);
TINY_API bool TinyCondition_NotifyOne(TinyCondition *thiz);
TINY_API bool TinyCondition_NotifyAll(TinyCondition *thiz);


TINY_END_DECLS

#endif /* __TINY_CONDITION_H__ */
