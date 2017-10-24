/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_log_impl.h
 *
 * @remark
 *
 */

#ifndef __TINY_LOG_IMPL_H__
#define __TINY_LOG_IMPL_H__


#include <android/log.h>

#ifdef TINY_DEBUG
    #define LOG_D(tag, format, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, format, ##__VA_ARGS__)
    #define LOG_E(tag, format, ...) __android_log_print(ANDROID_LOG_ERROR, tag, format, ##__VA_ARGS__)
    #define LOG_I(tag, format, ...) __android_log_print(ANDROID_LOG_INFO, tag, format, ##__VA_ARGS__)
    #define LOG_V(tag, format, ...) __android_log_print(ANDROID_LOG_VERBOSE, tag, format, ##__VA_ARGS__)
    #define LOG_W(tag, format, ...) __android_log_print(ANDROID_LOG_WARN, tag, format, ##__VA_ARGS__)
#else
    #define LOG_D(tag, format, ...)
    #define LOG_E(tag, format, ...) __android_log_print(ANDROID_LOG_ERROR, tag, format, ##__VA_ARGS__)
    #define LOG_I(tag, format, ...) __android_log_print(ANDROID_LOG_INFO, tag, format, ##__VA_ARGS__)
    #define LOG_V(tag, format, ...) __android_log_print(ANDROID_LOG_VERBOSE, tag, format, ##__VA_ARGS__)
    #define LOG_W(tag, format, ...) __android_log_print(ANDROID_LOG_WARN, tag, format, ##__VA_ARGS__)
#endif

#endif /* __TINY_LOG_IMPL_H__ */