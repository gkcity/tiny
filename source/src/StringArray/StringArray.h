/**
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   StringArray.h
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#ifndef __STRING_ARRAY_H__
#define __STRING_ARRAY_H__

#include <tiny_lor.h>
#include <TinyList.h>
#include "tiny_base.h"
#include "tiny_api.h"

TINY_BEGIN_DECLS


typedef struct _StringArray
{
    TinyList        values;
} StringArray;

TINY_API
TINY_LOR
StringArray * StringArray_NewString(const char *string, const char *separator);

TINY_API
TINY_LOR
void StringArray_Delete(StringArray *thiz);


TINY_END_DECLS

#endif /* __STRING_ARRAY_H__ */