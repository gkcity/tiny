/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_log.c
 *
 * @remark
 *
 */

#if defined(__ANDROID__)
    #include "android/tiny_log_impl.c"
#elif defined(ESP32)
    #include "esp32/tiny_log_impl.c"
#elif defined(ESP8266)
    #include "esp8266/tiny_log_impl.c"
#elif defined(MT7697)
    #include "mt7697/tiny_log_impl.c"
#elif defined(__OPENWRT_MT7688__)
    #include "openwrt_mt7688/tiny_log_impl.c"
#elif defined(__LINUX__)
    #include "linux/tiny_log_impl.c"
#elif defined(__WIN32__)
    #include "windows/tiny_log_impl.c"
#elif defined(__MACOS__)
    #include "macos/tiny_log_impl.c"
#else
    error "tiny_log not implemented!!!"
#endif