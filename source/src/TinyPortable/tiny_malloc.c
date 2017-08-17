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

void * tiny_malloc(uint32_t size)
{
    return umm_malloc(size);
}

void * tiny_realloc(void *p, uint32_t size)
{
    return umm_realloc(p, size);
}

void tiny_free(void *p)
{
    umm_free(p);
}
#else

void * tiny_malloc(uint32_t size)
{
    return malloc(size);
}

void * tiny_calloc(uint32_t n, size_t size)
{
    return calloc(n, size);
}

void * tiny_realloc(void *p, uint32_t size)
{
    return realloc(p, size);
}

void tiny_free(void *p)
{
    free(p);
}
#endif