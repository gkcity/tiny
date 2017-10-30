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


#ifdef TINY_DEBUG
    #define LOG_D(tag, format, ...) {printf(tag); printf("/D "); printf(format, ##__VA_ARGS__); printf("\n");}
    #define LOG_E(tag, format, ...) {printf(tag); printf("/E "); printf(format, ##__VA_ARGS__); printf("\n");}
    #define LOG_I(tag, format, ...) {printf(tag); printf("/I "); printf(format, ##__VA_ARGS__); printf("\n");}
    #define LOG_V(tag, format, ...) {printf(tag); printf("/V "); printf(format, ##__VA_ARGS__); printf("\n");}
    #define LOG_W(tag, format, ...) {printf(tag); printf("/W "); printf(format, ##__VA_ARGS__); printf("\n");}
#else
    #define LOG_D(tag, format, ...)
    #define LOG_E(tag, format, ...) {printf(tag); printf("/E "); printf(format, ##__VA_ARGS__); printf("\n");}
    #define LOG_I(tag, format, ...) {printf(tag); printf("/I "); printf(format, ##__VA_ARGS__); printf("\n");}
    #define LOG_V(tag, format, ...) {printf(tag); printf("/V "); printf(format, ##__VA_ARGS__); printf("\n");}
    #define LOG_W(tag, format, ...) {printf(tag); printf("/W "); printf(format, ##__VA_ARGS__); printf("\n");}
#endif


#endif /* __TINY_LOG_IMPL_H__ */