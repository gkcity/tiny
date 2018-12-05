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

#ifndef __TINY_MALLOC_ESP8266_H__
#define __TINY_MALLOC_ESP8266_H__

#include "tiny_base.h"
#include "tiny_lor.h"


#define tiny_malloc             malloc
#define tiny_calloc             calloc
#define tiny_realloc            realloc
#define tiny_free               free

#if 0
TINY_LOR
void * tiny_malloc(size_t size);

TINY_LOR
void * tiny_calloc(size_t n, size_t size);

TINY_LOR
void * tiny_realloc(void *p, size_t size);

TINY_LOR
void tiny_free(void *p);
#endif


#endif /* __TINY_MALLOC_ESP8266_H__ */
