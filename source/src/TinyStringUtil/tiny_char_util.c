/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   char_util.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include "tiny_char_util.h"
#include <ctype.h>

int is_char(int c)
{
    return c >= 0 && c <= 127;
}

int is_ctl(int c)
{
    return (c >= 0 && c <= 31) || (c == 127);
}

int is_tspecial(int c)
{
    switch (c)
    {
    case '(': case ')': case '<': case '>': case '@':
    case ',': case ';': case ':': case '\\': case '"':
    case '/': case '[': case ']': case '?': case '=':
    case '{': case '}': case ' ': case '\t':
        return 1;

    default:
        return 0;
    }
}

int is_digit(int c)
{
    return c >= '0' && c <= '9';
}

int tolower_compare(char a, char b)
{
    return (tolower(a) == tolower(b));
}
