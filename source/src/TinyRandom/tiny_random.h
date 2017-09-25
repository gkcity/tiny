/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   tiny_random.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __TINY_RANDOM_H__
#define __TINY_RANDOM_H__

#include <stdint.h>
#include <tiny_typedef.h>
#include <tiny_lor.h>

TINY_BEGIN_DECLS


TINY_LOR
void tiny_random_create(uint8_t *value, uint8_t length);


TINY_END_DECLS

#endif /* __TINY_RANDOM_H__ */