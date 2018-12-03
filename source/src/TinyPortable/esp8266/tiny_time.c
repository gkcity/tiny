/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   tiny_time.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include "tiny_time.h"
#include <sys/unistd.h>
#include <esp_system.h>
#include <tiny_snprintf.h>

TINY_LOR
uint64_t tiny_current_microsecond(void)
{
    struct timeval tv;

    memset(&tv, 0, sizeof(struct timeval));
    tiny_gettimeofday(&tv, NULL);

    return (uint64_t)(tv.tv_sec * 1000 * 1000 + tv.tv_usec);
}

TINY_LOR
int tiny_gettimeofday(struct timeval *tv, void *tz)
{
    return gettimeofday(tv, tz);
}

TINY_LOR
int tiny_getstrtime(char buf[], size_t len)
{
    tiny_snprintf(buf, len, "%d-%02d-%02d %02d:%02d:%02d.%03d", 0, 0, 0, 0, 0, 0, 0);

    return 0;
}
