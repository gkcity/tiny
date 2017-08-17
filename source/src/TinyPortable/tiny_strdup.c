/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_strdup.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include "tiny_strdup.h"
#include "tiny_malloc.h"

char * tiny_strdup(const char *src)
{
    char *dst = tiny_malloc((uint32_t) strlen(src) + 1);
    if (dst != NULL)
    {
        strcpy(dst, src);
    }

    return dst;
}