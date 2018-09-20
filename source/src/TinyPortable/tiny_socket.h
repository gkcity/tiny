/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_socket.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __TINY_SOCKET_H__
#define __TINY_SOCKET_H__


#if defined(__ANDROID__)
    #include "linux/tiny_socket.h"
#elif defined(ESP32)
    #include "esp32/tiny_socket.h"
#elif defined(ESP8266)
    #include "esp8266/tiny_socket.h"
#elif defined(__OPENWRT_MT7688__)
    #include "openwrt_mt7688/tiny_socket.h"
#elif defined(__LINUX__)
    #include "linux/tiny_socket.h"
#elif defined(__WIN32__)
    #include "windows/tiny_socket.h"
#elif defined(__MACOS__)
    #include "macos/tiny_socket.h"
#else
    error "tiny_socket not implemented!!!"
#endif



#endif /* __TINY_SOCKET_H__ */
