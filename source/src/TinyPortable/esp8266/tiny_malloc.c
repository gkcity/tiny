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
#if 0
#include "esp_system.h"

TINY_LOR
void * tiny_malloc(size_t size)
{
    void *v = malloc(size);
    if (v == NULL)
    {
        printf("tiny_malloc %d failed, free heap size: %d\n", (int)size, esp_get_free_heap_size());
    }

    return v;
}

TINY_LOR
void * tiny_calloc(size_t n, size_t size)
{
    void *v = calloc(n, size);
    if (v == NULL)
    {
        printf("tiny_calloc %d failed, free heap size: %d\n", (int)size, esp_get_free_heap_size());
    }

    return v;
}

TINY_LOR
void * tiny_realloc(void *p, size_t size)
{
    void *v = realloc(p, size);
    if (v == NULL)
    {
        printf("tiny_realloc %d failed, free heap size: %d\n", (int)size, esp_get_free_heap_size());
    }

    return v;
}

TINY_LOR
void tiny_free(void *p)
{
    free(p);
}
#endif