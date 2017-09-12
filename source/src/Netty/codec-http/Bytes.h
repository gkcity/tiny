/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   Bytes.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#ifndef __BYTES_H__
#define __BYTES_H__

#include <tiny_base.h>
#include <tiny_api.h>
#include <tiny_lor.h>

TINY_BEGIN_DECLS


typedef struct _Bytes
{
    const char  * value;
    uint32_t      offset;
    uint32_t      length;
} Bytes;

typedef Bytes Line;

TINY_API
TINY_LOR
bool Bytes_GetLine(Bytes *thiz, Line *line);


TINY_END_DECLS

#endif /* __BYTES_H__ */
