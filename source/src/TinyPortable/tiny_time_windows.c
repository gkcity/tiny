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

time_t tiny_time(time_t *t)
{
    return time(t);
}

#if 1
int tiny_gettimeofday(struct timeval *tv, void *tz)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;

    GetLocalTime(&wtm);
    tm.tm_year     = wtm.wYear - 1900;
    tm.tm_mon      = wtm.wMonth - 1;
    tm.tm_mday     = wtm.wDay;
    tm.tm_hour     = wtm.wHour;
    tm.tm_min      = wtm.wMinute;
    tm.tm_sec      = wtm.wSecond;
    tm.tm_isdst    = -1;
    clock = mktime(&tm);
    tv->tv_sec = (long)clock;
    tv->tv_usec = wtm.wMilliseconds * 1000;

    return (0);
}
#endif

#if 0
int tiny_gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}
#endif

#if 0
int tiny_sleep(int second)
{
    Sleep(second * 1000);
	return 0;
}

int tiny_usleep(int usecond)
{
    return Sleep(usecond);
}
#endif