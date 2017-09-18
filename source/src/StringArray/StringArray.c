/**
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   StringArray.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include <tiny_malloc.h>
#include "StringArray.h"

TINY_LOR
static void onValueDelete (void * data, void *ctx)
{
    tiny_free(data);
}

TINY_LOR
static TinyRet StringArray_Parse(StringArray *thiz, const char *string, const char *separator)
{
    TinyRet ret = TINY_RET_OK;




    return ret;
}

TINY_LOR
static TinyRet StringArray_Construct(StringArray *thiz, const char *string, const char *separator)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        memset(thiz, 0, sizeof(StringArray));
        ret = TinyList_Construct(&thiz->values);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyList_SetDeleteListener(&thiz->values, onValueDelete, NULL);

        ret = StringArray_Parse(thiz, string, separator);
    } while (false);

    return ret;
}

TINY_LOR
static void StringArray_Dispose(StringArray *thiz)
{
    TinyList_Dispose(&thiz->values);
    memset(thiz, 0, sizeof(StringArray));
}

TINY_LOR
StringArray * StringArray_NewString(const char *string, const char *separator)
{
    StringArray *thiz = NULL;

    RETURN_VAL_IF_FAIL(string, NULL);
    RETURN_VAL_IF_FAIL(separator, NULL);

    do
    {
        thiz = (StringArray *)tiny_malloc(sizeof(StringArray));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(StringArray_Construct(thiz, string, separator)))
        {
            StringArray_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TINY_LOR
void StringArray_Delete(StringArray *thiz)
{
    StringArray_Dispose(thiz);
    tiny_free(thiz);
}