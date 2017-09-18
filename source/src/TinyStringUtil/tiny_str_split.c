/**
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   str_split.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "tiny_str_split.h"
#include <ctype.h>

TINY_LOR
uint32_t tiny_str_split(const char *str, const char *c, char group[][128], uint32_t groupCount)
{
    uint32_t len = 128;
    uint32_t i = 0;
    uint32_t copy_len = 0;
    const char *p1 = NULL;
    const char *p2 = NULL;

    p1 = str;
    while (p1 != NULL)
    {
        p2 = strstr(p1, c);
        copy_len = (uint32_t) ((p2 != NULL) ? p2 - p1 : strlen(p1));

        if (copy_len > len)
        {
            copy_len = len;
        }

        if (copy_len > 0)
        {
            strncpy(group[i], p1, copy_len);
        }

        i++;

        if (i >= groupCount)
        {
            break;
        }

        if (p2 == NULL)
        {
            break;
        }

        p1 = p2 + strlen(c);
    }

    return i;
}