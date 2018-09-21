/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ByteProcessor.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */
#include "Bytes.h"

TINY_LOR
bool Bytes_GetLine(Bytes *thiz, Line *line)
{
//    uint32_t i = 0;
//
//    if (thiz->length - thiz->offset == 0)
//    {
//        line->value = thiz->value + thiz->offset;
//        line->offset = 0;
//        line->length = 0;
//        return true;
//    }

    for (uint32_t i = thiz->offset; i < (thiz->length - 1); ++i)
    {
        if (thiz->value[i] == '\r' && thiz->value[i+1] == '\n')
        {
            line->value = thiz->value + thiz->offset;
            line->offset = 0;
            line->length = i - thiz->offset;
            thiz->offset = i + 2;
            return true;
        }
    }

//    if (thiz->value[i + 1] == '\0')
//    {
//        line->value = thiz->value + thiz->offset;
//        line->offset = 0;
//        line->length = i - thiz->offset;
//        thiz->offset = i + 1;
//        return true;
//    }

    return false;
}