/**
 * Copyright (C) 2013-2015
 *
 * a memory allocator for embedded systems
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   umm_malloc.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 *
 */

#ifndef __UMM_MALLOC_H__
#define __UMM_MALLOC_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS

#if 0
extern UMM_HEAP_INFO heapInfo;
extern char   __umm_heap_start[];
extern char   __umm_heap_end[];
extern size_t __umm_heap_size;
#endif

void * umm_info(void *ptr, int force);
void * umm_malloc(size_t size);
void * umm_realloc(void *ptr, size_t size);
void umm_free(void *ptr);


TINY_END_DECLS

#endif /* __UMM_MALLOC_H__ */