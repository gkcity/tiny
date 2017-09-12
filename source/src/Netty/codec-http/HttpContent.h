/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpContent.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#ifndef __HTTP_CONTENT_H__
#define __HTTP_CONTENT_H__

#include <tiny_lor.h>
#include <tiny_base.h>
#include "Bytes.h"

TINY_BEGIN_DECLS


#define HTTP_CONTENT_MAX_SIZE   (1024 * 4)

typedef struct _HttpContent
{
    char       * buf;
    uint32_t     buf_size;
    uint32_t     data_size;
} HttpContent;

TINY_API
TINY_LOR
TinyRet HttpContent_Construct(HttpContent *thiz);

TINY_API
TINY_LOR
TinyRet HttpContent_Dispose(HttpContent *thiz);

TINY_API
TINY_LOR
TinyRet HttpContent_SetSize(HttpContent *thiz, uint32_t size);

TINY_API
TINY_LOR
uint32_t HttpContent_LoadBytes(HttpContent *thiz, Bytes *bytes);

#define HttpContent_IsFull(thiz)    ((thiz)->buf_size == (thiz)->data_size)


TINY_END_DECLS

#endif /* __HTTP_CONTENT_H__ */