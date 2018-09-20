/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_snprintf.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __TINY_SNPRINTF_H__
#define __TINY_SNPRINTF_H__


#if defined(__ANDROID__)
    #include "linux/tiny_snprintf.h"
#elif defined(ESP32)
    #include "esp32/tiny_snprintf.h"
#elif defined(ESP8266)
    #include "esp8266/tiny_snprintf.h"
#elif defined(__OPENWRT_MT7688__)
    #include "openwrt_mt7688/tiny_snprintf.h"
#elif defined(__LINUX__)
    #include "linux/tiny_snprintf.h"
#elif defined(__WIN32__)
    #include "windows/tiny_snprintf.h"
#elif defined(__MACOS__)
    #include "macos/tiny_snprintf.h"
#else
    error "tiny_snprintf not implemented!!!"
#endif


#endif /* __TINY_SNPRINTF_H__ */