/**
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   tiny_buffer_append.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include <tiny_log.h>
#include "tiny_buffer_append.h"

#define TAG     "tiny_buffer_append"

TINY_LOR
int tiny_buffer_append_byte(uint8_t *buf, uint32_t length, uint32_t offset, uint8_t value)
{
    if (buf != NULL)
    {
        if ((length - offset) < 1)
        {
            LOG_E(TAG, "buffer size too small");
            return 0;
        }

        buf[offset] = value;
    }

    return 1;
}

TINY_LOR
int tiny_buffer_append_bytes(uint8_t *buf, uint32_t length, uint32_t offset, uint8_t *value, uint32_t size)
{
    if (buf != NULL)
    {
        if (size > (int) (length - offset))
        {
            LOG_E(TAG, "buffer size too small");
            return 0;
        }

        memcpy(buf + offset, value, (size_t)size);
    }

    return size;
}

TINY_LOR
int tiny_buffer_append_string(char *buf, uint32_t length, uint32_t offset, const char *string)
{
    return tiny_buffer_append_bytes((uint8_t *)buf, length, offset, (uint8_t *)string, (uint32_t)strlen(string));

//    int size = strlen(string);
//
//
//    if (buf != NULL)
//    {
//        if (size > (int)(length - offset))
//        {
//            LOG_E(TAG, "buffer size too small");
//            return 0;
//        }
//
//        strncpy(buf + offset, string, (size_t)size);
//    }
//
//    return size;
}
