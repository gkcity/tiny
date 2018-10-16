/**
 * Copyright (C) 2013-2015
 *
 * @author coding.tom@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_malloc.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 *
 */

#include "tiny_malloc.h"

#ifdef TINY_UMM
#include "umm_malloc.h"

TINY_LOR
void * tiny_malloc(uint32_t size)
{
    return umm_malloc(size);
}

TINY_LOR
void * tiny_realloc(void *p, uint32_t size)
{
    return umm_realloc(p, size);
}

TINY_LOR
void tiny_free(void *p)
{
    umm_free(p);
}

#else

TINY_LOR
void * tiny_malloc(size_t size)
{
    return malloc(size);
}

TINY_LOR
void * tiny_calloc(size_t n, size_t size)
{
    return calloc(n, size);
}

TINY_LOR
void * tiny_realloc(void *p, size_t size)
{
    return realloc(p, size);
}

TINY_LOR
void tiny_free(void *p)
{
    free(p);
}
#endif