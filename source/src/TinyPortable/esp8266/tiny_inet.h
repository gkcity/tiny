/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_inet.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __TINY_INET_ESP8266_H__
#define __TINY_INET_ESP8266_H__


#ifdef LWIP_SOCKET
    #include <lwip/inet.h>
#else
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif


#endif /* __TINY_INET_ESP8266_H__ */
