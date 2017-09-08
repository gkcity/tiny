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
#include <espressif/esp_system.h>

TINY_LOR
uint64_t tiny_current_microsecond(void)
{
    return system_get_time();
}

TINY_LOR
int tiny_gettimeofday(struct timeval *tv, void *tz)
{
    return gettimeofday(tv, tz);
}