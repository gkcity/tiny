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
#include <sys/time.h>

time_t tiny_time(time_t *t)
{
    return time(t);
}

int tiny_gettimeofday(struct timeval *tv, void *tz)
{
    return gettimeofday(tv, tz);
}

#if 0
int tiny_sleep(int second)
{
    return sleep((unsigned int)second);
}

int tiny_usleep(int usecond)
{
    return usleep(usecond);
}
#endif