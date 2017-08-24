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

//long tiny_time(void *t)
//{
//    return (long) (0x1111000022223333);
//}

#if 1
TINY_LOR
time_t tiny_time(time_t *t)
{
    return micros();

    // multiple definition of `time' in liblwip.a
//    return time(t);
}

TINY_LOR
int tiny_gettimeofday(struct timeval *tv, void *tz)
{
    return gettimeofday(tv, tz);
}
#endif

#if 0
int tiny_sleep(int second)
{
    return sleep((unsigned int)second);
}

int tiny_usleep(int usecond)
{
    return (int) usleep((unsigned int)usecond);
}
#endif