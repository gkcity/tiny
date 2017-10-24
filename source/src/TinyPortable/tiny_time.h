/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   tiny_time.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __TINY_TIME_H__
#define __TINY_TIME_H__

#include <tiny_typedef.h>
#include <tiny_lor.h>
#include <tiny_api.h>

#if defined(__ANDROID__)
    #include "linux/tiny_time.h"
#elif defined(ESP32)
    #include "esp32/tiny_time.h"
#elif defined(ESP8266)
    #include "esp8266/tiny_time.h"
#elif defined(__LINUX__)
    #include "linux/tiny_time.h"
#elif defined(__WIN32__)
    #include "windows/tiny_time.h"
#else
    error "tiny_time not implemented!!!"
#endif

TINY_BEGIN_DECLS

TINY_API
TINY_LOR
int tiny_gettimeofday(struct timeval *tv, void *tz);

TINY_API
TINY_LOR
uint64_t tiny_current_microsecond(void);

TINY_API
TINY_LOR
int tiny_getstrtime(char buf[], size_t len);


TINY_END_DECLS

#endif /* __TINY_TIME_H__ */
