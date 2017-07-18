/**
 * Copyright (C) 2013-2015
 *
 * @author wenzhenwei@xiaomi.com
 * @date   2013-11-19
 *
 * @file   Miot.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __MIOT_H__
#define __MIOT_H__

#include <tiny_typedef.h>
#include <tiny_ret.h>
#include <tiny_api.h>
#include "channel/Channel.h"
#include "bootstrap/Bootstrap.h"

#include "miio.h"

TINY_BEGIN_DECLS
;
typedef void(*CloudCallback)(const char* msg, int len);

typedef struct _Miot
{
    Bootstrap bootstrap;
    MiioCtx miio;

    MiioCloudCtx* cloud;
    CloudCallback cloudCallback;

//    Channel* cloudClient;
//    Channel* smartServer;
//    Timer* syncTimer
//    Timer* ackTimer;
    
} Miot;

TINY_API Miot* Miot_New();
TINY_API TinyRet Miot_Construct(Miot* thiz);
TINY_API TinyRet Miot_SetDInfo(Miot* thiz, uint64_t did, const char* model, const char* vendor, const char* key);
TINY_API TinyRet Miot_SetDToken(Miot* thiz, const char* token);
TINY_API TinyRet Miot_SetNetInfo(Miot* thiz, const char* info);
TINY_API TinyRet Miot_Start(Miot* thiz);
TINY_API TinyRet Miot_Stop(Miot* thiz);
TINY_API void Miot_Dispose(Miot* thiz);
TINY_API void Miot_Delete(Miot* thiz);

TINY_END_DECLS

#endif /* __BOOTSTRAP_H__ */

