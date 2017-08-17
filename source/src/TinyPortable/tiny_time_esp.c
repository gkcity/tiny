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

int tiny_gettimeofday(struct timeval *tv, void *tz)
{
    return gettimeofday(tv, tz);
}

int tiny_sleep(int second)
{
    return sleep((unsigned int)second);
}

#if 0
int tiny_usleep(int usecond)
{
    return (int) usleep((unsigned int)usecond);
}
#endif