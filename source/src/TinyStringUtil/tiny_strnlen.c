/**
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   tiny_strnlen.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "tiny_strnlen.h"

TINY_LOR
size_t tiny_strnlen(const char *s, size_t maxlen)
{
    size_t ret = strlen(s);

    return (ret <= maxlen) ? ret : maxlen;
}