/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_log_print.c
 *
 * @remark
 *
 */

#include <tiny_time.h>
#include <tiny_snprintf.h>
#include "tiny_log_print.h"

#define LOG_HEAD                128
#define LOG_BUF_SIZE            1024
#define LOG_DATE_LEN            64

#define LOG_COLOR_NONE          "\033[m"
#define LOG_COLOR_RED           "\033[0;32;31m"
#define LOG_COLOR_LIGHT_RED     "\033[1;31m"
#define LOG_COLOR_GREEN         "\033[0;32;32m"
#define LOG_COLOR_LIGHT_GREEN   "\033[1;32m"
#define LOG_COLOR_BLUE          "\033[0;32;34m"
#define LOG_COLOR_LIGHT_BLUE    "\033[1;34m"
#define LOG_COLOR_DARY_GRAY     "\033[1;30m"
#define LOG_COLOR_CYAN          "\033[0;36m"
#define LOG_COLOR_LIGHT_CYAN    "\033[1;36m"
#define LOG_COLOR_PURPLE        "\033[0;35m"
#define LOG_COLOR_LIGHT_PURPLE  "\033[1;35m"
#define LOG_COLOR_BROWN         "\033[0;33m"
#define LOG_COLOR_YELLOW        "\033[1;33m"
#define LOG_COLOR_LIGHT_GRAY    "\033[0;37m"
#define LOG_COLOR_WHITE         "\033[1;37m"

#ifndef ESP

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
static void change_screen_color(int prio)
{
    const char *color = LOG_COLOR_NONE;

    switch (prio)
    {
    case TINY_LOG_VERBOSE:
        color = LOG_COLOR_DARY_GRAY;
        break;

    case TINY_LOG_DEBUG:
        color = LOG_COLOR_GREEN;
        break;

    case TINY_LOG_INFO:
        color = LOG_COLOR_LIGHT_GRAY;
        break;

    case TINY_LOG_WARN:
        color = LOG_COLOR_YELLOW;
        break;

    case TINY_LOG_ERROR:
        color = LOG_COLOR_RED;
        break;

    default:
        break;
    }

    printf("%s", color);
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

#ifdef _WIN32
#else
    change_screen_color(prio);
#endif

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

#endif /* ESP */