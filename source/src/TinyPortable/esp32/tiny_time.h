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

#ifndef __TINY_TIME_ESP32_H__
#define __TINY_TIME_ESP32_H__


#if LWIP_TIMEVAL_PRIVATE
    #include <lwip/sockets.h>
    #include <apps/time.h>
#else
    #include <sys/time.h>
#endif


#endif /* __TINY_TIME_ESP32_H__ */
