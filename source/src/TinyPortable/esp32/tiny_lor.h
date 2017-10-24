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

#ifndef __TINY_LOR_ESP32_H__
#define __TINY_LOR_ESP32_H__


/**
 * copy from <espressif/c_types.h>
 */

#define TINY_LOR            __attribute__((section(".irom0.text")))
#define TINY_LOR_VAR        __attribute__((section(".irom.text")))


#endif /* __TINY_LOR_ESP32_H__ */