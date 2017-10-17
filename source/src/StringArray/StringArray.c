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
#include <tiny_log.h>
#include "StringArray.h"

#define TAG         "StringArray"

TINY_LOR
static void onValueDelete (void * data, void *ctx)
{
    tiny_free(data);
}

TINY_LOR
static TinyRet StringArray_Parse(StringArray *thiz, const char *string, const char *separator)
{
    TinyRet ret = TINY_RET_OK;
    const char *p1 = NULL;
    const char *p2 = NULL;

    p1 = string;

    while (p1 != NULL)
    {
        uint32_t length = 0;
        char *value = NULL;

        p2 = strstr(p1, separator);
        length = (uint32_t) ((p2 != NULL) ? p2 - p1 : strlen(p1));

        value = tiny_malloc(length + 1);
        if (value == NULL)
        {
            LOG_E(TAG, "tiny_malloc FAILED");
            ret = TINY_RET_E_NEW;
            break;
        }

        memset(value, 0, length + 1);
        strncpy(value, p1, length);

        ret = TinyList_AddTail(&thiz->values, value);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyList_AddTail FAILED");
            ret = TINY_RET_E_NEW;
            break;
        }

        if (p2 == NULL)
        {
            break;
        }

        p1 = p2 + strlen(separator);
    }

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
            LOG_E(TAG, "TinyList_Construct FAILED");
            break;
        }

        TinyList_SetDeleteListener(&thiz->values, onValueDelete, NULL);

        if (string != NULL && separator != NULL)
        {
            ret = StringArray_Parse(thiz, string, separator);
            if (RET_FAILED(ret))
            {
                LOG_E(TAG, "StringArray_Parse FAILED");
            }
        }
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
            LOG_E(TAG, "tiny_malloc FAILED");
            break;
        }

        if (RET_FAILED(StringArray_Construct(thiz, string, separator)))
        {
            LOG_E(TAG, "StringArray_Construct FAILED");
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