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

#if defined(__ANDROID__)
    #include "linux/tiny_time.c"
#elif defined(ESP32)
    #include "esp32/tiny_time.c"
#elif defined(ESP8266)
    #include "esp8266/tiny_time.c"
#elif defined(__OPENWRT_MT7688__)
    #include "openwrt_mt7688/tiny_time.c"
#elif defined(__LINUX__)
    #include "linux/tiny_time.c"
#elif defined(__WIN32__)
    #include "windows/tiny_time.c"
#elif defined(__MACOS__)
    #include "macos/tiny_time.c"
#else
    error "tiny_time not implemented!!!"
#endif