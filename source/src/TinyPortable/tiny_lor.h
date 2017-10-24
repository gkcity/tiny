/**
 * Copyright (C) 2013-2015
 *
 * @author coding.tom@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_lor.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 *
 */

#ifndef __TINY_LOR_H__
#define __TINY_LOR_H__

/**
 * TINY_LOR = Load On Runtime
 */

#if defined(__ANDROID__)
    #include "linux/tiny_lor.h"
#elif defined(ESP32)
    #include "esp32/tiny_lor.h"
#elif defined(ESP8266)
    #include "esp8266/tiny_lor.h"
#elif defined(__LINUX__)
    #include "linux/tiny_lor.h"
#elif defined(__WIN32__)
    #include "windows/tiny_lor.h"
#else
    error "tiny_lor not implemented!!!"
#endif


#endif /* __TINY_LOR_H__ */