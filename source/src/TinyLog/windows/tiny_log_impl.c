/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_log_impl.c
 *
 * @remark
 *
 */

#include <tiny_time.h>
#include <tiny_snprintf.h>
#include "tiny_log.h"

#define LOG_HEAD                128
#define LOG_BUF_SIZE            1024
#define LOG_DATE_LEN            64


TINY_LOR
static char get_priority(int prio)
{
    char c = 'U';

    switch (prio)
    {
    case TINY_LOG_VERBOSE:
        c = 'V';
        break;

    case TINY_LOG_DEBUG:
        c = 'D';
        break;

    case TINY_LOG_INFO:
        c = 'I';
        break;

    case TINY_LOG_WARN:
        c = 'W';
        break;

    case TINY_LOG_ERROR:
        c = 'E';
        break;

    default:
        break;
    }

    return c;
}

TINY_LOR
int __tiny_log_write(int prio, const char *tag, const char *text)
{
    char log[LOG_HEAD + LOG_BUF_SIZE];
    char date[128] = {0};

    tiny_getstrtime(date, 128);

    tiny_snprintf(log,
            LOG_HEAD + LOG_BUF_SIZE,
            "[%s] %c/%s %s\n",
            date,
            get_priority(prio),
            tag,
            text);

    log[LOG_HEAD + LOG_BUF_SIZE - 1] = 0;

    printf("%s", log);

    return 0;
}

TINY_LOR
int __tiny_log_print(int prio, const char *tag,  const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    return __tiny_log_write(prio, tag, buf);
}

TINY_LOR
int __tiny_log_vprint(int prio, const char *tag, const char *fmt, va_list ap)
{
    char buf[LOG_BUF_SIZE];

    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);

    return __tiny_log_write(prio, tag, buf);
}