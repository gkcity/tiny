/**
 * Copyright (C) 2013-2015
 *
 * @author coding.tom@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_malloc.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 *
 */

#ifndef __TINY_MALLOC_LINUX_H__
#define __TINY_MALLOC_LINUX_H__

#include <tiny_lor.h>
#include <tiny_base.h>
#include <tiny_typedef.h>

TINY_BEGIN_DECLS


TINY_LOR
void * tiny_malloc(size_t size);

TINY_LOR
void * tiny_calloc(size_t n, size_t size);

TINY_LOR
void * tiny_realloc(void *p, size_t size);

TINY_LOR
void tiny_free(void *p);



TINY_END_DECLS

#endif /* __TINY_MALLOC_LINUX_H__ */