/**
 *
 * Copyright (C) 2007-2012 coding.tom
 *
 * @author jxfengzi@gmail.com
 * @date   2013-5-25
 *
 * @file   TinyMapItem.h
 *
 * @version 2013.8.6

 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __TINY_MAP_ITEM_H__
#define __TINY_MAP_ITEM_H__

#include "tiny_base.h"
#include "TinyContainerListener.h"
#include "TinyList.h"
#include <tiny_lor.h>

TINY_BEGIN_DECLS


#define TINY_MAP_MAX_KEY_LEN             128

typedef struct _TinyMapItem
{
    char      * key;
    void      * value;
} TinyMapItem;

TINY_API
TINY_LOR
TinyMapItem * TinyMapItem_New(const char *key, void *value);

TINY_API
TINY_LOR
void TinyMapItem_Delete(TinyMapItem *thiz);


TINY_END_DECLS

#endif /* __TINY_MAP_ITEM_H__ */