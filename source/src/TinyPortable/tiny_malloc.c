/**
 * Copyright (C) 2013-2015
 *
 * @author coding.tom@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_malloc.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 *
 */

#if defined(__ANDROID__)
    #include "linux/tiny_malloc.c"
#elif defined(ESP32)
    #include "esp32/tiny_malloc.c"
#elif defined(ESP8266)
    #include "esp8266/tiny_malloc.c"
#elif defined(__OPENWRT_MT7688__)
    #include "openwrt_mt7688/tiny_malloc.c"
#elif defined(__LINUX__)
    #include "linux/tiny_malloc.c"
#elif defined(__WIN32__)
    #include "windows/tiny_malloc.c"
#elif defined(__MACOS__)
    #include "macos/tiny_malloc.c"
#else
    error "tiny_malloc not implemented!!!"
#endif