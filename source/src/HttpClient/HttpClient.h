/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpClient.h
 *
 * @remark
 *
 */

#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include <tiny_base.h>
#include <bootstrap/Bootstrap.h>
#include "HttpExchange.h"

TINY_BEGIN_DECLS


typedef struct _HttpClient
{
    Channel         * client;
    Bootstrap         bootstrap;
} HttpClient;

TINY_API
TINY_LOR
HttpClient * HttpClient_New(void);

TINY_API
TINY_LOR
void HttpClient_Delete(_IN_ HttpClient *thiz);

TINY_API
TINY_LOR
TinyRet HttpClient_Send(_IN_ HttpClient *thiz, _IN_ HttpExchange *exchange);


TINY_END_DECLS

#endif /* __HTTP_CLIENT_H__ */