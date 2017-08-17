/**
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   tiny_str_equal.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "tiny_str_equal.h"
#include <ctype.h>

#if 0
TINY_LOR
int str_get_value(const char *buf, uint32_t buf_len, const char *start_tag, const char *end_tag, char *value, uint32_t value_len)
{
    const char *v_start = NULL;
    const char *start = NULL;
    const char *end = NULL;
    uint32_t v_len = 0;
    uint32_t len = 0;

    start = strstr(buf, start_tag);
    if (start == NULL)
    {
        return 0;
    }

    v_start = start + strlen(start_tag);
    len = v_start - buf;

    if (len > buf_len)
    {
        return 0;
    }

    if (end_tag == NULL)
    {
        v_len = buf_len - len;
    }
    else
    {
        end = strstr(v_start, end_tag);
        if (end == NULL)
        {
            return 0;
        }

        v_len = end - v_start;
    }

    if (v_len > value_len)
    {
        v_len = value_len;
    }

    strncpy(value, v_start, v_len);

    return v_len;
}
#endif