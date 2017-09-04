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

TINY_LOR
int is_char(int c)
{
    return c >= 0 && c <= 127;
}

TINY_LOR
int is_ctl(int c)
{
    return (c >= 0 && c <= 31) || (c == 127);
}

TINY_LOR
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

TINY_LOR
int is_digit(int c)
{
    return c >= '0' && c <= '9';
}

TINY_LOR
bool is_hex(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

//TINY_LOR
//int tolower_compare(char a, char b)
//{
//    return (tolower(a) == tolower(b));
//}
