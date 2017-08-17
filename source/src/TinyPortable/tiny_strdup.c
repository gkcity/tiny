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

TINY_LOR
char * tiny_strdup(const char *src)
{
    uint32_t len = (uint32_t)strlen(src);
    char *dst = (char *) tiny_malloc(len + 1);
    if (dst != NULL)
    {
        memset(dst, 0, len + 1);
        strncpy(dst, src, len);
    }

    return dst;
}
