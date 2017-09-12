/**
 *
 * Copyright (C) 2007-2012 coding.tom
 *
 * @author jxfengzi@gmail.com
 * @date   2013-5-25
 *
 * @file   TinyMapItem.c
 *
 * @version 2013.8.6

 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include "TinyMapItem.h"
#include <tiny_malloc.h>
#include <tiny_log.h>

#define TAG     "TinyMapItem"

TINY_LOR
static TinyRet TinyMapItem_Construct(TinyMapItem *thiz, const char *key, void *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        uint32_t keyLength = strlen(key) + 1;

        memset(thiz, 0, sizeof(TinyMapItem));

        thiz->key = (char *) tiny_malloc(keyLength);
        if (thiz->key == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        memset(thiz->key, 0, keyLength);
        strncpy(thiz->key, key, keyLength);

        thiz->value = value;
    } while (0);

    return ret;
}

TINY_LOR
static void TinyMapItem_Dispose(TinyMapItem *thiz)
{
    if (thiz->key != NULL)
    {
        tiny_free(thiz->key);
    }
}

TINY_LOR
TinyMapItem * TinyMapItem_New(const char *key, void *value)
{
    TinyMapItem * thiz = NULL;

    do
    {
        thiz = (TinyMapItem *)tiny_malloc(sizeof(TinyMapItem));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(TinyMapItem_Construct(thiz, key, value)))
        {
            LOG_E(TAG, "TinyMapItem_Construct FAILED");
            TinyMapItem_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (false);

    return thiz;
}

TINY_LOR
void TinyMapItem_Delete(TinyMapItem *thiz) 
{
    TinyMapItem_Dispose(thiz);
    tiny_free(thiz);
}