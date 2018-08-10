/**
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   str_equal.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "tiny_str_equal.h"
#include <ctype.h>

TINY_LOR
bool str_equal(const char *dst, const char *src, bool ignore_case)
{
    bool ret = true;

    do
    {
        if (dst == NULL || src == NULL)
        {
            ret = false;
            break;
        }

        if (!ignore_case)
        {
            ret = STR_EQUAL(dst, src);
            break;
        }

        while (*dst)
        {
            if (tolower(*dst++) != tolower(*src++))
            {
                ret = false;
                break;
            }
        }

        if (*dst != *src)
        {
            ret = false;
        }
    } while (0);

    return ret;
}
