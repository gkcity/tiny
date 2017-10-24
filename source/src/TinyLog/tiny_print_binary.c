/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   tiny_log_binary.c
 *
 * @remark
 *
 */

#include "tiny_print_binary.h"

#ifdef TINY_DEBUG
TINY_LOR
void tiny_print_binary(const char *title, const uint8_t *data, size_t len, bool wrapLines)
{
    printf("[ %s, %d ] = ", title, (int)len);

    if (wrapLines)
    {
        printf("\n");
    }

    for (size_t j = 0; j < len; ++j)
    {
        printf("%02X", data[j]);
        if ((j + 1) % 32 == 0)
        {
            if (wrapLines)
            {
                printf("\n");
            }
        }
        else
        {
            if (! ((j + 1) % 4))
            {
                if (wrapLines)
                {
                    printf(" ");
                }
            }
        }
    }
    printf("\n");
}
#endif