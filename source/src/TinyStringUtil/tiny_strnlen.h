/**
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   tiny_strnlen.h
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#ifndef __TINY_STRNLEN_H__
#define __TINY_STRNLEN_H__

#include <tiny_lor.h>
#include "tiny_base.h"
#include "tiny_api.h"

TINY_BEGIN_DECLS


TINY_API
TINY_LOR
size_t tiny_strnlen(const char *s, size_t maxlen);


TINY_END_DECLS

#endif /* __TINY_STRNLEN_H__ */
