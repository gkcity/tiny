/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   tiny_print_mem.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __TINY_PRINT_MEM_H__
#define __TINY_PRINT_MEM_H__

#include <tiny_typedef.h>
#include <tiny_lor.h>
#include <tiny_api.h>

TINY_BEGIN_DECLS



void tiny_print_mem(const char *tag, const char *function);

#define LOG_MEM(T,F)            tiny_print_mem(T,F)


TINY_END_DECLS

#endif /* __TINY_PRINT_MEM_H__ */
